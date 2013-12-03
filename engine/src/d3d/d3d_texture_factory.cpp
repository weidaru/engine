#include "d3d_texture_factory.h"
#include "engine.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <d3dx11.h>
#undef ERROR

#include <glog/logging.h>
#include <assert.h>

D3DTexture2D::D3DTexture2D() {
	texture = 0;
	sr_view = 0;
	rt_view = 0;
}

D3DTexture2D::~D3DTexture2D() {
	if(texture)
		texture->Release();
	if(sr_view)
		texture->Release();
	if(rt_view)
		texture->Release();
}

D3DTextureFactory::D3DTextureFactory(ID3D11Device *device) : device_(device) {

}

D3DTextureFactory::~D3DTextureFactory() {
	for(Texture2DMap::iterator it = data_2d_.begin(); it != data_2d_.end(); it++)
		delete it->second;
}

D3DTexture2D * D3DTextureFactory::CreateTexture2DFromFile(
						const std::string &name, 
						const std::string &file_path,
						const D3DX11_IMAGE_LOAD_INFO* info = NULL) {
	if(data_2d_.find(name) != data_2d_.end()) {
		LOG(WARNING)<<"Attempt to create 2d texture "<<name<<" multiple times. Creation is ignored.";
		return data_2d_[name];
	}

	D3DTexture2D *texture = new D3DTexture2D;
	ID3D11Resource *temp = 0;
	HRESULT result =  D3DX11CreateTextureFromFile(device_, name.c_str(), 0, 0, &temp, 0);
	temp->QueryInterface<ID3D11Texture2D>(&(texture->texture));
	CHECK(!FAILED(result) && texture->texture) << "Cannot create texture from file "<<file_path <<". Error "<<GetLastError();
	data_2d_[name] = texture;
	return texture;
}

D3DTexture2D * D3DTextureFactory::GetTexture2D(const std::string &name) {
	if(data_2d_.find(name) == data_2d_.end())
		return NULL;
	else
		return data_2d_[name];
}

D3DTexture2D * D3DTextureFactory::BindShaderResource(D3DTexture2D *texture) {
	D3D11_TEXTURE2D_DESC texture_desc;
	texture->texture->GetDesc(&texture_desc);
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;

	srv_desc.Format = texture_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels = 1;

	device_->CreateShaderResourceView(texture->texture, &srv_desc, &texture->sr_view);
	return texture;
}

D3DTexture2D * D3DTextureFactory::BindRenderTarget(D3DTexture2D *texture) {
	D3D11_TEXTURE2D_DESC texture_desc;
	texture->texture->GetDesc(&texture_desc);

	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
	rtv_desc.Format = texture_desc.Format;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D.MipSlice = 0;

	device_->CreateRenderTargetView(texture->texture, &rtv_desc, &texture->rt_view);
	return texture;
}

void D3DTextureFactory::RemoveTexture2D(const std::string &name) {
	if(data_2d_.find(name) != data_2d_.end()) {
		delete data_2d_[name];
		data_2d_.erase(name);
	}
}