#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")


#include "d3d11_graphic_resource_manager.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include "graphics/renderer/texture1d.h"
#include "graphics/renderer/texture3d.h"

#include "d3d11_texture2d.h"
#include "d3d11_texture_cube.h"
#include "d3d11_graphic_buffer.h"
#include "d3d11_shader_bytecode.h"
#include "d3d11_vertex_shader.h"
#include "d3d11_pixel_shader.h"
#include "d3d11_sampler.h"
#include "d3d11_geometry_shader.h"
#include "d3d11_input_layout.h"



namespace s2 {

D3D11GraphicResourceManager::D3D11GraphicResourceManager(ID3D11Device *_device)
		: device(_device){
	CHECK_NOTNULL(device);
}

#define MapClean(Type, name) \
for(std::map<uint32_t, Type *>::iterator it=name.begin(); it!=name.end(); it++) { \
	delete it->second; \
}\
name.clear()

D3D11GraphicResourceManager::~D3D11GraphicResourceManager() {
	MapClean(D3D11GraphicBuffer, buffer_map);
	MapClean(Texture1D, tex1d_map);
	MapClean(D3D11Texture2D, tex2d_map);
	MapClean(D3D11TextureCube, texcube_map);
	MapClean(Texture3D, tex3d_map);
	MapClean(D3D11Sampler, sampler_map);
	MapClean(D3D11ShaderBytecode, bytecode_map);
	MapClean(D3D11VertexShader, vs_map);
	MapClean(D3D11PixelShader, ps_map);
	MapClean(D3D11GeometryShader, gs_map);
	MapClean(D3D11InputLayout, input_layout_map);

	//Report memory leak.
	ID3D11Debug* debug;
	device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug));
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	debug->Release();

	device->Release();
	
}

#define Mangle(type) D3D11##type

#define ResourceImp(Type, name) \
Mangle(Type) * D3D11GraphicResourceManager::Create##Type() { \
	Mangle(Type) *temp = new Mangle(Type)(this); \
	name[temp->GetID()] = temp; \
	return temp; \
} \
void D3D11GraphicResourceManager::Remove##Type(uint32_t id) { \
	if(name.find(id) != name.end()) { \
		delete name[id]; \
		name.erase(id); \
	} \
} \
Mangle(Type) * D3D11GraphicResourceManager::Get##Type(uint32_t id) { \
	if(name.find(id) == name.end()) { \
		return 0; \
	} else { \
		return name[id]; \
	} \
}

ResourceImp(GraphicBuffer, buffer_map)
ResourceImp(Texture2D, tex2d_map)
ResourceImp(TextureCube, texcube_map)
ResourceImp(Sampler, sampler_map)
ResourceImp(ShaderBytecode, bytecode_map)
ResourceImp(VertexShader, vs_map)
ResourceImp(PixelShader, ps_map)
ResourceImp(GeometryShader, gs_map)
ResourceImp(InputLayout, input_layout_map)

Texture1D * D3D11GraphicResourceManager::CreateTexture1D() {
	CHECK(false)<<"Disabled.";
	return 0;
}

Texture1D * D3D11GraphicResourceManager::GetTexture1D(uint32_t id) {
	CHECK(false)<<"Disabled.";
	return 0;
}

void D3D11GraphicResourceManager::RemoveTexture1D(uint32_t id) {
	CHECK(false)<<"Disabled.";
}

Texture3D * D3D11GraphicResourceManager::CreateTexture3D() {
	CHECK(false)<<"Disabled.";
	return 0;
}

Texture3D * D3D11GraphicResourceManager::GetTexture3D(uint32_t id) {
	CHECK(false)<<"Disabled.";
	return 0;
}

void D3D11GraphicResourceManager::RemoveTexture3D(uint32_t id) {
	CHECK(false)<<"Disabled.";
}
}







