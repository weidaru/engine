#pragma comment(lib, "d3d11.lib")

#include "d3d11_graphic_buffer.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>
#include <cppformat/format.h>

#include "d3d11_graphic_resource_manager.h"
#include "d3d11_graphic_pipeline.h"
#include "d3d11_resource_helper.h"
#include "d3d11_mapped_resource.h"
#include "d3d11_resource_view.h"

#ifdef NDEBUG
	#define NiceCast(Type, Ptr) static_cast<Type>(Ptr)
#else
	#define NiceCast(Type, Ptr) dynamic_cast<Type>(Ptr)
#endif

namespace s2 {

D3D11GraphicBuffer::D3D11GraphicBuffer(D3D11GraphicResourceManager *_manager)
	:	manager(_manager), buffer(0), mapped(0),
		index_buffer(0), vertex_buffer(0), stream_out(0), unordered_access(0) {
}

D3D11GraphicBuffer::~D3D11GraphicBuffer() {
	Clean();
}

void D3D11GraphicBuffer::Clean() {
	delete unordered_access;
	delete stream_out;
	delete vertex_buffer;
	delete index_buffer;
	delete mapped;
	if(buffer) {
		buffer->Release();
	}
}

void D3D11GraphicBuffer::Check() const {
	CHECK(buffer) << "Vertex buffer is not initialized.";
}

bool D3D11GraphicBuffer::Initialize(const Option &option) {
	if(buffer != 0) {
		error = "Cannot initialize a buffer twice.";
		return false;
	}

	D3D11_BUFFER_DESC desc;
	D3D11ResourceHelper::SetBufferDesc(&desc, option.element_count * option.element_bytewidth, option.resource_write);
	desc.BindFlags = 0;
	if (option.binding & RendererBinding::VERTEX_BUFFER) {
		desc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
	}
	if (option.binding & RendererBinding::INDEX_BUFFER)  {
		desc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
	}
	if (option.binding & RendererBinding::STREAM_OUT) {
		desc.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
	}
	if(option.binding & RendererBinding::UNORDERED_ACCESS) {
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		desc.StructureByteStride = option.element_bytewidth;
	}

	HRESULT result = 1;
	if (option.data) {
		D3D11_SUBRESOURCE_DATA subresource;
		subresource.pSysMem = option.data;
		result = manager->GetDevice()->CreateBuffer(&desc, &subresource, &buffer);
	} else {
		result = manager->GetDevice()->CreateBuffer(&desc, 0, &buffer);
	}

	if(FAILED(result)) {
		error = fmt::format("{}{}", "Cannot create buffer. Error code: ", ::GetLastError()); 
		Clean();
		return false;
	}

	mapped = new D3D11MappedResource(buffer, option.resource_write);

	//Create all the views.
	if (option.binding & RendererBinding::VERTEX_BUFFER) {
		vertex_buffer = new D3D11VertexBuffer(this);
	}
	if (option.binding & RendererBinding::INDEX_BUFFER) {
		index_buffer = new D3D11IndexBuffer(this);
	} 
	if (option.binding & RendererBinding::STREAM_OUT) {
		stream_out = new D3D11StreamOut(this);
	}
	if(option.binding & RendererBinding::UNORDERED_ACCESS) {
		//Create the view.
		D3D11_UNORDERED_ACCESS_VIEW_DESC  uav_desc;
		uav_desc.Format = DXGI_FORMAT_UNKNOWN;
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav_desc.Buffer.FirstElement = 0;
		uav_desc.Buffer.NumElements = 0;
		uav_desc.Buffer.Flags = 0;

		ID3D11UnorderedAccessView *view = 0;
		result = manager->GetDevice()->CreateUnorderedAccessView(buffer, &uav_desc, &view);
		if(FAILED(result)) {
			error = fmt::format("{}{}",  "Cannot create UnorderedAccessView. Error code: ", ::GetLastError());
			Clean();
			return false;
		}

		unordered_access = new D3D11UnorderedAccess(this, view);
	}
	this->option = option;
	return true;
}

const GraphicBuffer::Option & D3D11GraphicBuffer::GetOption() const {
	return option;
}

void D3D11GraphicBuffer::WriteMap(GraphicPipeline *_pipeline, bool no_overwrite) {
	Check();
	D3D11GraphicPipeline *pipeline = NiceCast(D3D11GraphicPipeline *, _pipeline);
	if(_pipeline) {
		CHECK(pipeline)<<"Error casting pipeline to D3D11GraphicPipeline";
	}

	mapped->WriteMap(pipeline, no_overwrite, 0);
}

void D3D11GraphicBuffer::WriteUnmap() {
	Check();
	mapped->WriteUnmap();
}

void D3D11GraphicBuffer::Write(uint32_t index, const void *data, uint32_t array_size, uint32_t element_bytewidth) {
	Check();

	mapped->Write(index*element_bytewidth, data, array_size*element_bytewidth);
}

const void * D3D11GraphicBuffer::Read(uint32_t index, uint32_t element_bytewidth) const {
	Check();

	return (const char *)mapped->Read() + index*element_bytewidth;
}

void D3D11GraphicBuffer::ReadMap(GraphicPipeline *_pipeline, bool wipe_cache) const {
	Check();
	D3D11GraphicPipeline *pipeline = NiceCast(D3D11GraphicPipeline *, _pipeline);
	if(_pipeline) {
		CHECK(pipeline)<<"Error casting pipeline to D3D11GraphicPipeline";
	}

	//Create the staging resource if not present.
	if (mapped->GetStagingResource() == 0) {
		D3D11_BUFFER_DESC desc;
		desc.ByteWidth = option.element_bytewidth*option.element_count;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		HRESULT result = 1;
		ID3D11Buffer *staging_resource;
		result = manager->GetDevice()->CreateBuffer(&desc, 0, &staging_resource);
		CHECK(!FAILED(result)) << "Cannot create staging resource. Error " << ::GetLastError();
		mapped->SetStagingResource(staging_resource);
	}
	mapped->ReadMap(pipeline, 0, wipe_cache);
}

void D3D11GraphicBuffer::ReadUnmap() const {
	Check();
	mapped->ReadUnmap();
}

void D3D11GraphicBuffer::Update(GraphicPipeline * _pipeline, 
			uint32_t index, const void *data, uint32_t array_size, uint32_t element_bytewidth) {
	Check();
	D3D11GraphicPipeline *pipeline = NiceCast(D3D11GraphicPipeline *, _pipeline);
	if(_pipeline) {
		CHECK(pipeline)<<"Error casting pipeline to D3D11GraphicPipeline";
	}

	D3D11_BOX dest;
	dest.left = index*element_bytewidth;
	dest.right = (index + array_size)*element_bytewidth;
	dest.top = 0;
	dest.bottom = 1;
	dest.front = 0;
	dest.back = 1;

	pipeline->GetDeviceContext()->UpdateSubresource(
		buffer, 0, &dest, data, 0, 0);
}

IndexBuffer * D3D11GraphicBuffer::AsIndexBuffer() const {
	CHECK(index_buffer != 0)<<"Buffer is not binded as index buffer";
	return index_buffer;
}

VertexBuffer * D3D11GraphicBuffer::AsVertexBuffer() const {
	CHECK(vertex_buffer != 0) << "Buffer is not binded as vertex buffer";
	return vertex_buffer;
}

StreamOut * D3D11GraphicBuffer::AsStreamOut() const {
	CHECK(stream_out != 0) << "Buffer is not binded as stream out";
	return stream_out;
}

UnorderedAccess * D3D11GraphicBuffer::AsUnorderedAccess() const {
	CHECK(unordered_access != 0) << "Buffer is not binded as unordered access view";
	return unordered_access;
}

}







