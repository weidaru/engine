#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_texture2d.h"

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_enum_converter.h"
#include "d3d11_graphic_resource_manager.h"

namespace s2 {

namespace {

bool VerifyOption(const Texture2D::Option &option, s2string *error) {
	if(option.as_rendertarget &&  option.as_depthstencil) {
		*error = "Cannot bind as both RenderTarget and DepthStencil.";
		return false;
	}
	if(option.width<=0 || option.height<=0) {
		*error = "width and height must be greater than 0";
		return false;
	}
	if(option.sample_size<=0 || option.array_size<=0) {
		*error = "sample_size and array_size must be greater than 0";
		return false;
	}
	return true;
} 

}


D3D11Texture2D::D3D11Texture2D(D3D11GraphicResourceManager *_manager) 
		: manager(_manager), tex(0), ds_view(0), rt_view(0), sr_view(0) {
	CHECK(_manager != 0)<<"Manager should never be NULL";
}

D3D11Texture2D::~D3D11Texture2D() {
	Clear();
}

void D3D11Texture2D::Clear() {
	if(tex) {
		tex->Release();
		tex = 0;
	}
	if(ds_view) {
		ds_view->Release();
		ds_view = 0;
	}
	if(rt_view) {
		rt_view->Release();
		rt_view = 0;
	}
	if(sr_view) {
		sr_view->Release();
		sr_view = 0;
	}
}

void D3D11Texture2D::InitAsBackBuffer(ID3D11Texture2D *_tex, ID3D11RenderTargetView *_rt_view,const Option &_option) {
	CHECK(_tex && _rt_view)<<"Parameters must not be NULL";
	VerifyOption(_option, &error);
	Clear();
	
	tex = _tex;
	rt_view = _rt_view;
	
}

bool D3D11Texture2D::Initialize(const Texture2D::Option &_option) {
	if(!VerifyOption(_option, &error)) {
		return false;
	}
	Clear();

	tex = 0;
	rt_view = 0;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = _option.width;
	desc.Height = _option.height;
	desc.MipLevels = _option.mip_level;
	desc.ArraySize = _option.array_size;
	desc.Format = D3D11EnumConverter::TextureFormatToDXGIFormat(_option.format);
	
	desc.SampleDesc.Count = _option.sample_size;
	desc.SampleDesc.Quality = _option.sample_size>1?D3D11_STANDARD_MULTISAMPLE_PATTERN : 0;
	
	D3D11_RENDER_TARGET_VIEW_DESC *rtv_desc=0;
	D3D11_DEPTH_STENCIL_VIEW_DESC *dsv_desc=0;
	D3D11_SHADER_RESOURCE_VIEW_DESC *srv_desc = 0;
	if(_option.as_rendertarget) {
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
	else if(_option.as_depthstencil) {
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsv_desc = new D3D11_DEPTH_STENCIL_VIEW_DESC;
		dsv_desc->Format = desc.Format;
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
		
	if(_option.as_shaderresource) {
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
	if(_option.is_dynamic) {
		desc.Usage =  D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	
	desc.MiscFlags = 0;
	
	HRESULT result=1;
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = _option.data;
		data.SysMemPitch = _option.width*TextureEnum::GetFormatSize(_option.format);
		result = manager->GetDevice()->CreateTexture2D(&desc, &data, &tex);
		CHECK(!FAILED(result))<<"Cannot create texture 2d. Error " << ::GetLastError();
	}
	
	if(rtv_desc) {
		result = manager->GetDevice()->CreateRenderTargetView(tex, rtv_desc, &rt_view);
		CHECK(!FAILED(result))<<"Cannot create render target view. Error " << ::GetLastError();
		delete rtv_desc;
	}
	if(srv_desc) {
		result = manager->GetDevice()->CreateDepthStencilView(tex, dsv_desc, &ds_view);
		CHECK(!FAILED(result))<<"Cannot create render target view. Error " << ::GetLastError();
		delete srv_desc;
	}
	if(srv_desc) {
		result = manager->GetDevice()->CreateShaderResourceView(tex, srv_desc, &sr_view);
		CHECK(!FAILED(result))<<"Cannot create shader resource view. Error " << ::GetLastError();
		delete srv_desc;
	}
	return true;
}

void * D3D11Texture2D::Map() {
	CHECK(tex)<<"Texture not initialized.";
	D3D11_MAPPED_SUBRESOURCE subresource;
	HRESULT result = 1;
	result = manager->GetDeviceContext()->Map(tex, 1, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	CHECK(!FAILED(result))<<"Fail to map texture 2d. Error code "<<::GetLastError();
	
	return subresource.pData;
}

void D3D11Texture2D::UnMap() {
	CHECK(tex)<<"Texture not initialized.";
	manager->GetDeviceContext()->Unmap(tex, 0);
}

void D3D11Texture2D::GetOption(Texture2D::Option *option) {
	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);
	
	option->width = desc.Width;
	option->height = desc.Height;
	option->mip_level = desc.MipLevels;
	option->array_size= desc.ArraySize;
	option->format = D3D11EnumConverter::DXGIFormatToTextureFormat(desc.Format);
	option->sample_size = desc.SampleDesc.Count;
	option->is_dynamic = (desc.Usage == D3D11_USAGE_DYNAMIC);
	option->as_rendertarget = (rt_view!=0);
	option->as_depthstencil = (ds_view!=0);
	option->as_shaderresource = (sr_view!=0);
}

void D3D11Texture2D::GetLastError(s2string *str) {
	*str = error;
}

}



