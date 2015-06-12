#include "d3d11_texture_cube.h"
#include "d3d11_enum_converter.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_graphic_pipeline.h"
#include "d3d11_mapped_resource.h"

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

D3D11TextureCube::D3D11TextureCube(D3D11GraphicResourceManager *_manager)
		:	manager(_manager), tex(0), mapped(0),
			shader_resource(0){
	CHECK_NOTNULL(_manager);
}

D3D11TextureCube::~D3D11TextureCube() {
	delete shader_resource;
	delete mapped;
	if(tex) {
		tex->Release();
	}
}

void D3D11TextureCube::Check() const {
	CHECK(tex) << "Texture2D is not initialized.";
}

namespace {

void SetDesc(const TextureCube::Option &option, D3D11_TEXTURE2D_DESC *_desc) {
	D3D11_TEXTURE2D_DESC &desc = *_desc;
	
	desc.Width = option.width;
	desc.Height = option.height;
	desc.MipLevels = option.mip_level;
	desc.ArraySize = option.array_size * 6;
	desc.Format = D3D11EnumConverter::TextureFormatToDXGIFormat(option.format);
	desc.SampleDesc.Count = 0;
	desc.SampleDesc.Quality = 0;
	
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
}

}

void D3D11TextureCube::Initialize(const Option &_option)  {
	CHECK(tex==0)<<"Cannot initalize twice.";

	option = _option;
	D3D11_TEXTURE2D_DESC desc;
	SetDesc(option, &desc);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC *srv_desc = 0;
	desc.BindFlags = 0;
	
	//Bind as Shader Resource.
	desc.BindFlags = desc.BindFlags | D3D11_BIND_SHADER_RESOURCE;
	srv_desc = new D3D11_SHADER_RESOURCE_VIEW_DESC;
	srv_desc->Format = desc.Format;
	if(desc.ArraySize > 1) {
		srv_desc->ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		srv_desc->TextureCubeArray.MipLevels = desc.MipLevels;
		srv_desc->TextureCubeArray.MostDetailedMip = 0;
		srv_desc->TextureCubeArray.First2DArrayFace = 0;
		srv_desc->TextureCubeArray.NumCubes = desc.ArraySize;
	} else {
		srv_desc->ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srv_desc->TextureCube.MipLevels = desc.MipLevels;
		srv_desc->TextureCube.MostDetailedMip = 0;
	}
	
	desc.CPUAccessFlags = 0;
	if(_option.resource_write == RendererEnum::CPU_WRITE_FREQUENT) {
		desc.Usage =  D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	} else if(_option.resource_write == RendererEnum::CPU_WRITE_OCCASIONAL) {
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	} else if(_option.resource_write == RendererEnum::IMMUTABLE) {
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	} else {
		CHECK(false)<<"Not supported.";
	}
	
	HRESULT result=1;
	if(_option.data) {
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = _option.data;
		data.SysMemPitch = _option.width*RendererEnum::GetFormatSize(_option.format);
		result = manager->GetDevice()->CreateTexture2D(&desc, &data, &tex);
	} else {
		result = manager->GetDevice()->CreateTexture2D(&desc, 0, &tex);
	}
		
	CHECK(!FAILED(result))<<"Cannot create texture 2d. Error " << ::GetLastError();
	
	if(srv_desc) {
		ID3D11ShaderResourceView *sr_view = 0;
		result = manager->GetDevice()->CreateShaderResourceView(tex, srv_desc, &sr_view);
		CHECK(!FAILED(result))<<"Cannot create shader resource view. Error " << ::GetLastError();
		shader_resource = new D3D11ShaderResource(this, sr_view);
		delete srv_desc;
	}

	mapped = new D3D11MappedResource(tex, _option.resource_write);
}

const TextureCube::Option & D3D11TextureCube::GetOption() const {
	Check();
	return option;
}
	
void D3D11TextureCube::WriteMap(
	GraphicPipeline *_pipeline, bool no_overwrite, 
	uint32_t mip_index, uint32_t array_index, CubeFace face) {
	Check();
	D3D11GraphicPipeline *pipeline = NiceCast(D3D11GraphicPipeline *, _pipeline);
	if(_pipeline) {
		CHECK(pipeline)<<"Error casting pipeline to D3D11GraphicPipeline";
	}

	mapped->WriteMap(pipeline, no_overwrite, D3D11CalcSubresource(mip_index, array_index*6+face, option.mip_level));
}

void D3D11TextureCube::Write(uint32_t row, uint32_t col,  const void *data, uint32_t size) {
	Check();
	
	mapped->Write(mapped->GetWriteRowPitch() + col*RendererEnum::GetFormatSize(option.format), data, size);
}

void D3D11TextureCube::WriteUnmap() {
	Check();

	mapped->WriteUnmap();
}
	
void D3D11TextureCube::ReadMap(
	GraphicPipeline *_pipeline, uint32_t mip_index, 
	uint32_t array_index, CubeFace face, bool wipe_cache) const {
	Check();
	D3D11GraphicPipeline *pipeline = NiceCast(D3D11GraphicPipeline *, _pipeline);
	if(_pipeline) {
		CHECK(pipeline)<<"Error casting pipeline to D3D11GraphicPipeline";
	}

	if(mapped->GetStagingResource() == 0) {
		D3D11_TEXTURE2D_DESC desc;
		SetDesc(option, &desc);
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		
		HRESULT result = 1;
		ID3D11Texture2D *staging_resource;
		result = manager->GetDevice()->CreateTexture2D(&desc, 0, &staging_resource);
		CHECK(!FAILED(result))<<"Cannot create stagng resource. Error"<<::GetLastError;
		mapped->SetStagingResource(staging_resource);
	}
	
	mapped->ReadMap(pipeline, D3D11CalcSubresource(mip_index, array_index*6+face, option.mip_level), wipe_cache);
}

const void * D3D11TextureCube::Read(uint32_t row, uint32_t col) const {
	Check();
	return (const char *)mapped->Read() + mapped->GetReadRowPitch() + col*RendererEnum::GetFormatSize(option.format);
}

void D3D11TextureCube::ReadUnmap() const {
	Check();
	mapped->ReadUnmap();
}
	
void D3D11TextureCube::Update(
		GraphicPipeline *_pipeline, 
		uint32_t mip_index, uint32_t array_index, CubeFace face,
		uint32_t left, uint32_t right,
		uint32_t top, uint32_t bottom,
		const void *data) {
	Check();
	CHECK(mapped->GetResourceWrite() == RendererEnum::CPU_WRITE_OCCASIONAL)<<
				"Only CPU_WRITE_OCCASIONAL is allowed to update.";
	D3D11GraphicPipeline *pipeline = NiceCast(D3D11GraphicPipeline *, _pipeline);
	if(_pipeline) {
		CHECK(pipeline)<<"Error casting pipeline to D3D11GraphicPipeline";
	}

	uint32_t ele_size = RendererEnum::GetFormatSize(option.format);
	D3D11_BOX dest;
	dest.left = left*ele_size;
	dest.right = right*ele_size;
	dest.top = top;
	dest.bottom = bottom;
	dest.front = 0;
	dest.back = 1;
	
	pipeline->GetDeviceContext()->UpdateSubresource(
		tex, D3D11CalcSubresource(mip_index, array_index*6+face, option.mip_level), &dest, (const void *)data, option.width*ele_size, 0 
	);
}

D3D11ShaderResource * D3D11TextureCube::AsShaderResource() const {
	CHECK(shader_resource != 0) << "Texture is not created as shader resource";
	return shader_resource;
}

}


