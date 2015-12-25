#pragma comment(lib, "d3d11.lib")

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "d3d11_geometry_shader.h"

#include "d3d11_graphic_resource_manager.h"
#include "d3d11_shader_reflection.h"

namespace s2 {

D3D11GeometryShader::D3D11GeometryShader(D3D11GraphicResourceManager *_manager)
	: manager(_manager), bytecode(0), delete_bytecode(false), shader(0){

}

D3D11GeometryShader::~D3D11GeometryShader() {
	if(shader) {
		shader->Release();
	}
	
	if(delete_bytecode) {
		delete bytecode;
	}
}

bool D3D11GeometryShader::Initialize(ShaderBytecode *_bytecode) {
	CHECK(bytecode==0)<<"Cannot initialize twice";
	bytecode = static_cast<D3D11ShaderBytecode *>(_bytecode);

	ID3DBlob *blob = bytecode->GetBlob();
	HRESULT result = 1;
	result = manager->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, &shader);
	if(FAILED(result)) {
		S2StringFormat(&error, "Cannot create pixel shader. Error code %d", ::GetLastError());
		return false;
	} else {
		return true;
	}
}

bool D3D11GeometryShader::Initialize(const s2string &path, const s2string &entry_point) {
	CHECK(bytecode==0)<<"Cannot initialize twice";
	D3D11ShaderBytecode *b = new D3D11ShaderBytecode(manager);
	if( b->Initialize(path, entry_point) == false ) {
		error = b->GetLastError();
		delete b;
		return false;
	} else {
		delete_bytecode = true;
		return Initialize(b);
	}
}

bool D3D11GeometryShader::Initialize(ShaderBytecode *_bytecode, 
		int rasterized_stream,  const std::vector<StreamOutDescriptor> &stream_outs) {
	CHECK(bytecode==0)<<"Cannot initialize twice";

	bytecode = static_cast<D3D11ShaderBytecode *>(_bytecode);

	const D3D11ShaderReflection &reflect = bytecode->GetReflection();
	std::vector<D3D11_SO_DECLARATION_ENTRY> entries;

	uint32_t size = reflect.GetOutputSize();

	for (uint32_t i = 0; i < size; i++) {

		const D3D11ShaderReflection::Parameter& output_param = reflect.GetOutput(i);

		int slot_index = -1;
		for (auto it = stream_outs.begin(); it != stream_outs.end(); it++) {
			if (output_param.stream == it->stream_index) {
				slot_index = it->index;
			}
		}
		if (slot_index != -1) {
			D3D11_SO_DECLARATION_ENTRY entry;
			entry.Stream = output_param.stream;
			entry.SemanticName = output_param.semantic.c_str();
			entry.SemanticIndex = output_param.semantic_index;
			entry.StartComponent = 0;
			entry.ComponentCount = output_param.size / TypeInfoManager::GetSingleton()->Get(output_param.type_name).GetSize();
			entry.OutputSlot = slot_index;
			entries.push_back(entry);
		}
	}

	unsigned real_rasterized_stream;
	if (rasterized_stream < 0) {
		real_rasterized_stream = D3D11_SO_NO_RASTERIZED_STREAM;
	}
	else {
		real_rasterized_stream = rasterized_stream;
	}

	ID3DBlob *blob = bytecode->GetBlob();
	HRESULT result = 1;
	result = manager->GetDevice()->CreateGeometryShaderWithStreamOutput(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&entries[0],
		entries.size(),
		0,
		0,
		real_rasterized_stream,
		0,
		&shader
		);
	if(FAILED(result)) {
		S2StringFormat(&error, "Fail to create streamout geometry shader error %d", GetLastError()); 
		return false;
	} else {
		return true;
	}
}

}