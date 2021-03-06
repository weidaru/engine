#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#include "d3d11_texture2d.h"

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_enum_converter.h"
#include "d3d11_graphic_resource_manager.h"
#include "d3d11_graphic_pipeline.h"
#include "d3d11_mapped_resource.h"

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {


D3D11Texture2D::D3D11Texture2D(D3D11GraphicResourceManager *_manager) 
		:	manager(_manager), tex(0), mapped(0),
			render_target(0), depth_stencil(0), shader_resource(0){
	CHECK_NOTNULL(_manager);
}

D3D11Texture2D::~D3D11Texture2D() {
	delete shader_resource;
	delete depth_stencil;
	delete render_target;
	delete mapped;
	if(tex) {
		tex->Release();
	}
}

void D3D11Texture2D::Check() const {
	CHECK(tex) << "Texture2D is not initialized.";
}


void D3D11Texture2D::InitAsBackBuffer(ID3D11Texture2D *_tex, ID3D11RenderTargetView *_rt_view,const Option &_option) {
	CHECK(_tex && _rt_view)<<"Parameters must not be NULL";
	
	tex = _tex;
	render_target = new D3D11RenderTarget(this, _rt_view);
	option = _option;
	
	mapped = new D3D11MappedResource(tex, _option.resource_write);
}

namespace {

void SetDesc(const Texture2D::Option &option, D3D11_TEXTURE2D_DESC *_desc) {
	D3D11_TEXTURE2D_DESC &desc = *_desc;
	
	desc.Width = option.width;
	desc.Height = option.height;
	desc.MipLevels = option.mip_level;
	desc.ArraySize = option.array_size;
	desc.Format = D3D11EnumConverter::TextureFormatToDXGIFormat(option.format);
	
	desc.SampleDesc.Count = option.sample_size;
	desc.SampleDesc.Quality = option.sample_size>1?D3D11_STANDARD_MULTISAMPLE_PATTERN : 0;
	
	desc.MiscFlags = 0;
}

}

void D3D11Texture2D::Initialize(const Texture2D::Option &_option) {
	CHECK(tex==0)<<"Cannot initalize twice.";

	option = _option;
	D3D11_TEXTURE2D_DESC desc;
	SetDesc(option, &desc);
	
	D3D11_RENDER_TARGET_VIEW_DESC *rtv_desc=0;
	D3D11_DEPTH_STENCIL_VIEW_DESC *dsv_desc=0;
	D3D11_SHADER_RESOURCE_VIEW_DESC *srv_desc = 0;
	desc.BindFlags = 0;

	if(_option.output_bind == TextureEnum::RENDER_TARGET) {
		desc.BindFlags = D3D11_BIND_RENDER_TARGET;
		rtv_desc = new D3D11_RENDER_TARGET_VIEW_DESC;
		
		rtv_desc->Format = desc.Format;
		if(desc.ArraySize > 1 && desc.SampleDesc.Count > 1) {
			rtv_desc->ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
			rtv_desc->Texture2DMSArray.FirstArraySlice = 0;
			rtv_desc->Texture2DMSArray.ArraySize = desc.ArraySize;
		}
		else if(desc.ArraySize > 1) {
			rtv_desc->ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtv_desc->Texture2DArray.MipSlice = 0;						//Always use the original texture
			rtv_desc->Texture2DArray.FirstArraySlice = 0;				//Always start from zero
			rtv_desc->Texture2DArray.ArraySize = desc.ArraySize;	
		}
		else if(desc.SampleDesc.Count > 1) {
			rtv_desc->ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else  {
			rtv_desc->ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtv_desc->Texture2D.MipSlice = 0;			//Always use the original texture.
		}
		
	}
	else if(_option.output_bind == TextureEnum::DEPTH_STENCIL) {
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsv_desc = new D3D11_DEPTH_STENCIL_VIEW_DESC;
		dsv_desc->Format = desc.Format;
		dsv_desc->Flags = 0;
		//dsv_dsc->Flags is useful when multiple depth stencil buffer is bound. Our assumption is only one for now.
		if(desc.ArraySize > 1 && desc.SampleDesc.Count > 1) {
			dsv_desc->ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
			dsv_desc->Texture2DMSArray.FirstArraySlice = 0;
			dsv_desc->Texture2DMSArray.ArraySize = desc.ArraySize;
		}
		else if(desc.ArraySize > 1) {
			dsv_desc->ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsv_desc->Texture2DArray.MipSlice = 0;						//Always use the original texture
			dsv_desc->Texture2DArray.FirstArraySlice = 0;				//Always start from zero
			dsv_desc->Texture2DArray.ArraySize = desc.ArraySize;	
		}
		else if(desc.SampleDesc.Count > 1) {
			dsv_desc->ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else  {
			dsv_desc->ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsv_desc->Texture2D.MipSlice = 0;			//Always use the original texture.
		}
	}
		
	if(_option.input_bind == TextureEnum::SHADER_RESOURCE) {
		desc.BindFlags = desc.BindFlags | D3D11_BIND_SHADER_RESOURCE;
		srv_desc = new D3D11_SHADER_RESOURCE_VIEW_DESC;
		srv_desc->Format = desc.Format;
		if(desc.ArraySize > 1 && desc.SampleDesc.Count > 1) {
			srv_desc->ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
			srv_desc->Texture2DMSArray.FirstArraySlice = 0;
			srv_desc->Texture2DMSArray.ArraySize = desc.ArraySize;
		}
		else if(desc.ArraySize > 1) {
			srv_desc->ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srv_desc->Texture2DArray.MostDetailedMip = 0;			//Always use the original texture
			srv_desc->Texture2DArray.MipLevels = desc.MipLevels;
			srv_desc->Texture2DArray.FirstArraySlice = 0;				//Always start from zero
			srv_desc->Texture2DArray.ArraySize = desc.ArraySize;	
		}
		else if(desc.SampleDesc.Count > 1) {
			srv_desc->ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
		else  {
			srv_desc->ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srv_desc->Texture2D.MostDetailedMip = 0;			//Always use the original texture.
			srv_desc->Texture2D.MipLevels = desc.MipLevels;
		}
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
	if(_option.data.IsEmpty() == false) {
		D3D11_SUBRESOURCE_DATA *sub_resources = new D3D11_SUBRESOURCE_DATA[_option.data.GetSize()];
		for(uint32_t i=0; i<_option.data.GetArraySize(); i++) {
			for(uint32_t j=0; j<_option.data.GetMipLevel(); j++) {
				D3D11_SUBRESOURCE_DATA &sub_resource = sub_resources[D3D11CalcSubresource(j, i, _option.mip_level)];
				sub_resource.pSysMem = _option.data.GetData(i, j);
				sub_resource.SysMemPitch = _option.width/(j+1)*RendererEnum::GetFormatSize(_option.format);
			}
		}
		result = manager->GetDevice()->CreateTexture2D(&desc, sub_resources, &tex);
		delete[] sub_resources;
	} else {
		result = manager->GetDevice()->CreateTexture2D(&desc, 0, &tex);
	}
		
	CHECK(!FAILED(result))<<"Cannot create texture 2d. Error " << ::GetLastError();
	
	if(rtv_desc) {
		ID3D11RenderTargetView *rt_view = 0;
		result = manager->GetDevice()->CreateRenderTargetView(tex, rtv_desc, &rt_view);
		CHECK(!FAILED(result))<<"Cannot create render target view. Error " << ::GetLastError();
		render_target = new D3D11RenderTarget(this, rt_view);
		delete rtv_desc;
	}
	if(dsv_desc) {
		ID3D11DepthStencilView *ds_view = 0;
		result = manager->GetDevice()->CreateDepthStencilView(tex, dsv_desc, &ds_view);
		CHECK(!FAILED(result))<<"Cannot create depth stencil view. Error " << ::GetLastError();
		depth_stencil = new  D3D11DepthStencil(this, ds_view);
		delete dsv_desc;
	}
	if(srv_desc) {
		ID3D11ShaderResourceView *sr_view = 0;
		result = manager->GetDevice()->CreateShaderResourceView(tex, srv_desc, &sr_view);
		CHECK(!FAILED(result))<<"Cannot create shader resource view. Error " << ::GetLastError();
		shader_resource = new D3D11ShaderResource(this, sr_view);
		delete srv_desc;
	}

	mapped = new D3D11MappedResource(tex, _option.resource_write);
}

void D3D11Texture2D::WriteMap(GraphicPipeline *_pipeline, bool no_overwrite, uint32_t mip_index, uint32_t array_index) {
	Check();
	D3D11GraphicPipeline *pipeline = NiceCast(D3D11GraphicPipeline *, _pipeline);
	if(_pipeline) {
		CHECK(pipeline)<<"Error casting pipeline to D3D11GraphicPipeline";
	}

	mapped->WriteMap(pipeline, no_overwrite, D3D11CalcSubresource(mip_index, array_index, option.mip_level));
}

void D3D11Texture2D::WriteUnmap() {
	Check();
	mapped->WriteUnmap();
}

void D3D11Texture2D::Write(uint32_t row, uint32_t col,  const void *data, uint32_t size) {
	Check();
	
	mapped->Write(mapped->GetWriteRowPitch() + col*RendererEnum::GetFormatSize(option.format), data, size);
}

void D3D11Texture2D::ReadMap(GraphicPipeline *_pipeline, uint32_t mip_index, uint32_t array_index, bool wipe_cache) const {
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
	
	mapped->ReadMap(pipeline, D3D11CalcSubresource(mip_index, array_index, option.mip_level), wipe_cache);
}

void D3D11Texture2D::ReadUnmap() const {
	Check();
	mapped->ReadUnmap();
}

const void * D3D11Texture2D::Read(uint32_t row, uint32_t col) const {
	Check();
	return (const char *)mapped->Read() + mapped->GetReadRowPitch() + col*RendererEnum::GetFormatSize(option.format);
}

const Texture2D::Option & D3D11Texture2D::GetOption() const {
	Check();
	return option;
}

void D3D11Texture2D::Update(
			GraphicPipeline *_pipeline,
			uint32_t mip_index, uint32_t array_index,
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
		tex, D3D11CalcSubresource(mip_index, array_index, option.mip_level), &dest, (const void *)data, option.width*ele_size, 0 
	);
}

D3D11RenderTarget * D3D11Texture2D::AsRenderTarget() const {
	CHECK(render_target != 0) << "Texture is not created as render target";

	return render_target;
}

D3D11DepthStencil * D3D11Texture2D::AsDepthStencil() const {
	CHECK(depth_stencil != 0) << "Texture is not created as depth stencil";

	return depth_stencil;
}

D3D11ShaderResource * D3D11Texture2D::AsShaderResource() const {
	CHECK(shader_resource != 0) << "Texture is not created as shader resource";

	return shader_resource;
}


}




