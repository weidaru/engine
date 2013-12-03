#include "d3d_buffer_factory.h"
#include "engine.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

D3DBufferFactory::D3DBufferFactory(ID3D11Device *device) : device_(device) {

}

D3DBufferFactory::~D3DBufferFactory() {
	for(Map::iterator it=data_.begin(); it != data_.end(); it++)
		it->second->Release();
}

ID3D11Buffer * D3DBufferFactory::CreateConstantBuffer(const std::string &name, unsigned int size, const void *data) {
	if(data_.find(name) != data_.end()) {
		LOG(WARNING)<<"Attempt to create constant buffer "<<name<<" multiple times. Creation is ignored.";
		return data_[name];
	}

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subresource;
	subresource.pSysMem = data;

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ID3D11Buffer *buffer = 0;
	HRESULT result = device_->CreateBuffer(&desc, &subresource, &buffer);
	CHECK(!FAILED(buffer)) << "Cannot create constant buffer "<<name<<". Error "<<GetLastError();
	data_[name] = buffer;
	return buffer;
}

ID3D11Buffer * D3DBufferFactory::CreateIndexBuffer(const std::string &name, unsigned int size, const void *data) {
	if(data_.find(name) != data_.end()) {
		LOG(WARNING)<<"Attempt to create index buffer "<<name<<" multiple times. Creation is ignored.";
		return data_[name];
	}
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subresource;
	subresource.pSysMem = data;

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ID3D11Buffer *buffer = 0;
	HRESULT result = device_->CreateBuffer(&desc, &subresource, &buffer);
	CHECK(!FAILED(buffer)) << "Cannot create index buffer "<<name<<". Error "<<GetLastError();
	data_[name] = buffer;
	return buffer;
}

ID3D11Buffer * D3DBufferFactory::CreateVertexBuffer(const std::string &name, unsigned int size, const void *data) {
	if(data_.find(name) != data_.end()) {
		LOG(WARNING)<<"Attempt to create vertex buffer "<<name<<" multiple times. Creation is ignored.";
		return data_[name];
	}
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subresource;
	subresource.pSysMem = data;

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ID3D11Buffer *buffer = 0;
	HRESULT result = device_->CreateBuffer(&desc, &subresource, &buffer);
	CHECK(!FAILED(buffer)) << "Cannot create vertex buffer "<<name<<". Error "<<GetLastError();
	data_[name] = buffer;
	return buffer;
}

ID3D11Buffer * D3DBufferFactory::Get(const std::string &name) {
	if(data_.find(name) == data_.end()) 
		return NULL;
	else
		return data_[name];
}

void D3DBufferFactory::Remove(const std::string &name) {
	if(data_.find(name) != data_.end()) {
		data_[name]->Release();
		data_.erase(name);
	}
}