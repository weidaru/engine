#pragma comment(lib, "d3d11.lib")


#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_input_layout.h"
#include "d3d11_vertex_shader.h"
#include "d3d11_shader_reflection.h"
#include "d3d11_graphic_resource_manager.h"

namespace s2 {

namespace {

//Exception for POSITION semantic 
DXGI_FORMAT GetFormat(const D3D11ShaderReflection::Parameter &p) {
	static DXGI_FORMAT float_list[4] = { DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT };
	static DXGI_FORMAT int_list[4] = { DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32G32_SINT,
		DXGI_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32A32_SINT };
	static DXGI_FORMAT uint_list[4] = { DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32G32_UINT,
		DXGI_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32A32_UINT };
	if (p.type_name == "float")
		return float_list[p.size / 4 - 1];
	if (p.type_name == "int")
		return int_list[p.size / 4 - 1];
	if (p.type_name == "uint")
		return uint_list[p.size / 4 - 1];
	CHECK(false) << "Unexpected parameter typename " << p.type_name;
	return DXGI_FORMAT_R32_FLOAT;
}

}


D3D11InputLayout::D3D11InputLayout(D3D11GraphicResourceManager *_manager)
	:manager(_manager), layout(0) {
	CHECK_NOTNULL(_manager);
}

D3D11InputLayout::~D3D11InputLayout() {
	if(layout) {
		layout->Release();
	}
}

bool D3D11InputLayout::InitializeWithElement(const std::vector<ElementDescriptor> &elements, const VertexShader &_shader)  {
	if(layout != 0) {
		error = "Cannot initlize twice.";
		return false;
	}
	const D3D11VertexShader &shader = static_cast<const D3D11VertexShader &>(_shader);
	const D3D11ShaderReflection &reflect = shader.GetReflection();
	
	uint32_t size = reflect.GetInputSize();
	if(elements.size() != size) {
		S2StringFormat(&error, "ElementDescriptor size %d does not match input size %d", elements.size(), size);
		return false;
	}
	D3D11_INPUT_ELEMENT_DESC *descs = new D3D11_INPUT_ELEMENT_DESC[size];
	for(uint32_t i=0; i<size; i++) {
		const D3D11ShaderReflection::Parameter &p = reflect.GetInput(i);
		D3D11_INPUT_ELEMENT_DESC &desc = descs[i];
		const ElementDescriptor &element = elements[i];
		desc.Format = GetFormat(p);
		desc.InputSlot = element.vertex_buffer_index;
		desc.AlignedByteOffset = element.offset;
		desc.SemanticName = p.semantic.c_str();
		desc.SemanticIndex = p.semantic_index;
		//All the instance semantics starts with Instance_
		if (p.semantic.substr(0, 9) == "INSTANCE_") {
			desc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			desc.InstanceDataStepRate = 1; 			//Make this 1 which means 1 data for 1 instance.
		}
		else {
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;
		}
	}

	ID3D11Device *device = manager->GetDevice();
	HRESULT result = 1;
	result = device->CreateInputLayout(descs, size, shader.GetBlob()->GetBufferPointer(), shader.GetBlob()->GetBufferSize(), &(layout));
	CHECK(!FAILED(result)) << "Fail to create input layout error " << ::GetLastError();
	delete[] descs;

	return true;
}

bool D3D11InputLayout::InitializeWithVertexBuffer(const std::vector<VertexBufferDescriptor> &vbs, const VertexShader &shader) {
	if(layout != 0) {
		error = "Cannot initlize twice.";
		return false;
	}

	int element_size = 0;
	for(uint32_t i=0; i<vbs.size(); i++) {
		const TypeInfo &type_info = TypeInfoManager::GetSingleton()->Get(vbs[i].type_name);
		element_size += type_info.GetMemberSize();
	}

	std::vector<ElementDescriptor> elements(element_size);

	int head = 0;
	for(uint32_t i=0; i<vbs.size(); i++) {
		const VertexBufferDescriptor &vb = vbs[i];
		const TypeInfo &type_info = TypeInfoManager::GetSingleton()->Get(vb.type_name);
		for(uint32_t j=0; j<type_info.GetMemberSize(); j++) {
			ElementDescriptor &e = elements[head];
			e.vertex_buffer_index = vb.index;
			e.offset = type_info.GetMemberOffset(j);
			head++;
		}
	}
	
	return InitializeWithElement(elements, shader);
}


}

