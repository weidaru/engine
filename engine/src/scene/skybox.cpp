#include "skybox.h"

#include "asset/mesh.h"
#include "asset/skybox_image.h"
#include "asset/asset_path.h"

#include <glog/logging.h>

#include "graphics/renderer/all.h"
#include "engine.h"

#include "entity/transform.h"
#include "scene/scene_manager.h"
#include "scene/camera.h"

namespace s2 {

Skybox::Skybox(EntitySystem *system, SkyboxImage *image)
	:	Entity(system), mesh(0), env_texture(0), sampler(0), 
		vb(0), ib(0), vs(0), ps(0), drawing_state(0){
	mesh = new Mesh();
	CHECK(mesh->Initialize(ResolveAssetPath("model/sphere.obj")))<<mesh->GetLastError();
	CHECK(image)<<"Image cannot be null.";
	GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();

	{
		uint32_t size = mesh->GetVertexSize();
		float (*vertices)[3]  = new float[size][3];

		for(uint32_t i=0; i<size; i++) {
			Mesh::Vertex v = mesh->GetVertex(i);
			vertices[i][0] = v.x;
			vertices[i][1] = v.y;
			vertices[i][2] = v.z;
		}

		vb = manager->CreateBuffer();
		Buffer::Option option;
		option.element_count = size;
		option.element_member_count = 3;
		option.element_bytewidth = sizeof(float)*3;
		option.data = vertices;
		option.resource_write = RendererEnum::IMMUTABLE;
		vb->Initialize(option);
		delete[] vertices;
	}
	
	{
		//Create IndexBuffer
		uint32_t size = mesh->GetTriangleSize()*3;
		Buffer::IndexBufferElementType* indices = new Buffer::IndexBufferElementType[size];
		for(uint32_t i=0; i<mesh->GetTriangleSize(); i++) {
			indices[i*3] = mesh->GetTriangleVertexIndex(i, 0);
			indices[i*3+1] = mesh->GetTriangleVertexIndex(i, 1);
			indices[i*3+2] = mesh->GetTriangleVertexIndex(i, 2);
		}
		ib = manager->CreateBuffer();
		Buffer::Option option;
		option.InitializeAsIndexBuffer(size, indices);
		option.resource_write = RendererEnum::IMMUTABLE;
		ib->Initialize(option);
		delete[] indices;
	}
	
	{
		env_texture = manager->CreateTextureCube();
		TextureCube::Option option;
		image->PopulateTextureCubeOption(&option);
		option.resource_write = RendererEnum::IMMUTABLE;
		env_texture->Initialize(option);
	}

	sampler = manager->CreateSampler();
	sampler->Initialize(Sampler::Option());

	vs = manager->CreateVertexShader();
	CHECK(vs->Initialize(ResolveAssetPath("shader/skybox.hlsl"), "vs_main"))<<vs->GetLastError();

	ps = manager->CreatePixelShader();
	CHECK(ps->Initialize(ResolveAssetPath("shader/skybox.hlsl"), "ps_main"))<<ps->GetLastError();
	ps->SetSampler("shader_sampler", sampler);
	ps->SetShaderResource("env_cube", env_texture->AsShaderResource());
}

Skybox::~Skybox() {
	GraphicResourceManager *manager = Engine::GetSingleton()->GetRendererContext()->GetResourceManager();
	manager->RemoveTextureCube(env_texture->GetID());
	manager->RemoveBuffer(ib->GetID());
	manager->RemoveBuffer(vb->GetID());
}


void Skybox::OneFrame(float delta) {
	GraphicPipeline *pipeline = Engine::GetSingleton()->GetRendererContext()->GetPipeline();


	Camera *camera = Engine::GetSingleton()->GetSceneManager()->GetCamera();
	Matrix4x4 wvp(Engine::GetSingleton()->GetSceneManager()->GetProjectionMatrix());
	Matrix4x4 temp;
	wvp *= camera->GetViewMatrix();
	temp.SetTranslate(camera->GetTransform()->GetTranslate());
	wvp *= temp;
	temp.SetScale(5.0f, 5.0f, 5.0f);
	wvp *= temp;

	vs->SetUniform("wvp", wvp);

	pipeline->Start();
	pipeline->PushState();

	RasterizationOption rast_opt = pipeline->GetRasterizationOption();
	rast_opt.cull_mode = RasterizationOption::NONE;
	pipeline->SetPrimitiveTopology(GraphicPipeline::TRIANGLE_LIST);
	pipeline->SetRasterizationOption(rast_opt);
	pipeline->SetVertexBuffer(0, 0, vb->AsVertexBuffer());
	pipeline->SetIndexBuffer(ib->AsIndexBuffer(), 0);
	pipeline->SetVertexShader(vs);
	pipeline->SetPixelShader(ps);
	pipeline->Draw(&drawing_state, 0, ib->GetElementCount());

	pipeline->PopState();
	pipeline->End();
}


}

