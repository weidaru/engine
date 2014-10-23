#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")


#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#undef ERROR

#include <glog/logging.h>

#include "engine_program.h"
#include "engine.h"

#include "renderer/graphic_pipeline.h"
#include "renderer/graphic_resource_manager.h"
#include "renderer/imp/directx11/d3d11_shader_reflection.h"
#include "renderer/texture2d.h"

#include <stdio.h>

namespace s2 {

class TestProgram : public EngineProgram {
public:
	TestProgram():ds_buffer(0) {}

	virtual ~TestProgram() {}
	virtual bool Initialize(){
		printf("Initialize test program.\n");
		
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
		GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
		
		//Back buffer as render target 
		pipeline->SetRenderTarget(0, manager->GetBackBuffer());
		
		//Create and set depth stencil buffer
		RendererSetting renderer_setting;
		Engine::GetSingleton()->GetRendererContext()->GetSetting(&renderer_setting);
		Texture2D::Option ds_option;
		Texture2D::Option::SetAsDepthStencilBuffer(&ds_option, renderer_setting.window_width, renderer_setting.window_height);
		Texture2D *ds_buffer = manager->CreateTexture2D(ds_option);
		pipeline->SetDepthStencilBuffer(ds_buffer);


		//Just compile from file for now.
		unsigned int flag = D3DCOMPILE_ENABLE_STRICTNESS;
		flag |= D3DCOMPILE_DEBUG;
		ID3DBlob* shader_blob = 0;
		ID3DBlob* error_blob = 0;
		HRESULT result = 1;
		s2string path("d:\\temp\\color.vs");

		{
			FILE* file = fopen(path.c_str(), "rb");
			fseek(file, 0, SEEK_END);
			long size = ftell(file);
			fseek(file, 0, SEEK_SET);
			char *buffer = new char[size];
			fread(buffer, 1, size, file);
			printf(buffer);
			result = D3DCompile(buffer, size, path.c_str(), 0, 0, "PerVertex", "vs_5_0", flag, 0, &shader_blob, &error_blob);
			delete[] buffer;
		}

		if(FAILED(result)) {
			s2string error  =(char *)(error_blob ? error_blob->GetBufferPointer() : "Fail to retrive error message.");
			if(shader_blob)
				shader_blob->Release();
			if(error_blob)
				error_blob->Release();
		}

		ID3D11ShaderReflection *reflect = 0;
		D3DReflect( shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), 
            IID_ID3D11ShaderReflection, (void**) &reflect);
		D3D11_SHADER_DESC desc;
		reflect->GetDesc(&desc);

		D3D11_SIGNATURE_PARAMETER_DESC in_desc;
		reflect->GetInputParameterDesc(1, &in_desc);

		ID3D11ShaderReflectionVariable* var =  reflect->GetVariableByName("myStruct");
		D3D11_SHADER_VARIABLE_DESC  var_desc;
		var->GetDesc(&var_desc);
		{
			D3D11_SHADER_TYPE_DESC type_desc;
			var->GetType()->GetDesc(&type_desc);
			int a = 0;
		}
		
		{
			D3D11_SHADER_TYPE_DESC type_desc;
			var->GetType()->GetMemberTypeByName("b")->GetDesc(&type_desc);
			var->GetType()->GetMemberTypeByName("a")->GetDesc(&type_desc);
			var->GetType()->GetMemberTypeByName("c")->GetDesc(&type_desc);
			var->GetType()->GetMemberTypeByName("foo")->GetDesc(&type_desc);
			int a = 0;
		}

		ID3D11ShaderReflectionConstantBuffer* cb =  reflect->GetConstantBufferByName("CBuffer1");
		D3D11_SHADER_BUFFER_DESC  cb_desc;
		cb->GetDesc(&cb_desc);
		{
			D3D11_SHADER_VARIABLE_DESC  var_desc;
			cb->GetVariableByName("array")->GetDesc(&var_desc);
			int dummyt = 0;
		}
		

		D3D11_SHADER_INPUT_BIND_DESC input_bind;
		reflect->GetResourceBindingDesc(2, &input_bind);

		D3D11ShaderReflection ref(path, shader_blob);
		bool check = ref.CheckCompatible("float[3]", TypeInfoManager::GetSingleton()->Get("float[3]"));
		{
			const TypeInfo & info = ref.GetTypeInfo("MyStruct");
			unsigned int size = info.GetSize();
			unsigned int member_count = info.GetMemberSize();
			for(unsigned int i=0; i<member_count; i++) {
				s2string member_name = info.GetMemberName(i);
				unsigned int offset = info.GetMemberOffset(i);
			}

			int dummy = 1;
		}
	 	
		
		return true;
	}
	
	virtual const s2string & GetName() {
		static s2string name="test";
		return name;
	}
	
	virtual void OneFrame(float delta) {
		GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();
	
		float red[4] = {1.0f, 0.0f, 0.0f, 1.0f};
		pipeline->SetRenderTargetClearOption(0, true, red);
		pipeline->Draw();
	}
	
private:
	Texture2D *ds_buffer;
};

AddBeforeMain(TestProgram)

}

