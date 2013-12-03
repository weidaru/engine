#include "d3d_input_layout_factory.h"

ID3D11InputLayout * CreateInputLayout(ID3D11ShaderReflection *reflect, ID3D10Blob *vs_buffer) {
	D3D11_SHADER_DESC desc ;
	reflect->GetDesc(&desc);
	unsigned int offset = 0;
	int n = desc->InputParameters;
	D3D11_INPUT_ELEMENT_DESC *element_desc  = new D3D11_INPUT_ELEMENT_DESC[n];
	for(int i=0; i<n; i++) {
		D3D11_SIGNATURE_PARAMETER_DESC param_desc;       
		reflect->GetInputParameterDesc(i, &param_desc );

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC &cur = element_desc[i];   
		cur.SemanticName = param_desc.SemanticName;      
		cur.SemanticIndex = param_desc.SemanticIndex;
		cur.InputSlot = 0;
		cur.AlignedByteOffset = offset;
		cur.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		cur.InstanceDataStepRate = 0;   

		// determine DXGI format
		if ( param_desc.Mask == 1 )
		{
			if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) cur.Format = DXGI_FORMAT_R32_UINT;
			else if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) cur.Format = DXGI_FORMAT_R32_SINT;
			else if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) cur.Format = DXGI_FORMAT_R32_FLOAT;
			offset += 4;
		}
		else if ( param_desc.Mask <= 3 )
		{
			if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) cur.Format = DXGI_FORMAT_R32G32_UINT;
			else if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) cur.Format = DXGI_FORMAT_R32G32_SINT;
			else if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) cur.Format = DXGI_FORMAT_R32G32_FLOAT;
			offset += 8;
		}
		else if ( param_desc.Mask <= 7 )
		{
			if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) cur.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) cur.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) cur.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			offset += 12;
		}
		else if ( param_desc.Mask <= 15 )
		{
			if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) cur.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) cur.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if ( param_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) cur.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			offset += 16;
		}
	}

	ID3D11InputLayout * input_layout = 0;
	HRESULT result = device->CreateInputLayout(
		element_desc,
		n, 
		vs_buffer->GetBufferPointer(), 
		vs_buffer->GetBufferSize(), 
		input_layout);

	return input_layput;
}