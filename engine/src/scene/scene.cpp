#include "scene.h"

#include "entity/entity.h"
#include "entity/entity_system.h"
#include "engine.h"

#include "graphics/material/material.h"
#include "graphics/material/material_system.h"

#include "asset/mesh.h"

#include <assimp/Importer.hpp>   
#include <assimp/scene.h> 
#include <assimp/postprocess.h>  

#include <glog/logging.h>

namespace s2 {

Scene::Scene() : importer(0) {

}

Scene::~Scene() {
	EntitySystem *system = Engine::GetSingleton()->GetEntitySystem();
	for(auto it=entities.begin(); it!=entities.end(); it++) {
		delete system->Remove(it->second->GetId());
	}
	
	for(auto it=mesh_data.begin(); it!=mesh_data.end(); it++) {
		delete it->second;
	}

	delete importer;
}

bool Scene::Initialize(const s2string &path) {
	importer = new Assimp::Importer;
	const aiScene* scene = importer->ReadFile( path, 
		aiProcess_CalcTangentSpace			| 
		aiProcess_Triangulate						|
		aiProcess_JoinIdenticalVertices		|
		aiProcess_FlipUVs 						|
		aiProcess_GenNormals
		);

	if(scene == 0) {
		error = importer->GetErrorString();
		return false;
	}
	if(scene->mRootNode == 0) {
		error = "No root node for scene " + path;
		return false;
	}

	bool succeed = ProcessNode(scene->mRootNode, scene);

	return succeed;
}

bool Scene::ProcessNode(aiNode *node, const aiScene *scene) {
	if(node->mParent == 0 && node->mNumChildren != 0) {
		for(uint32_t i=0; i<node->mNumChildren; i++) {
			if(ProcessNode(node->mChildren[i], scene) == false) {
				return false;
			}
		}
	} else {
		s2string name = node->mName.C_Str();
		Entity *e = new Entity(Engine::GetSingleton()->GetEntitySystem());
		CHECK(entities.find(name)==entities.end()) << "entity "<<name<<" already exists.";

		Material *material = new Material(e);
		Engine::GetSingleton()->GetMaterialSystem()->Register(material);
		for(uint32_t i=0; i<node->mNumMeshes; i++) {
			aiMesh *raw_mesh = scene->mMeshes[node->mMeshes[i]];
			auto it = mesh_data.find(raw_mesh);
			if(it != mesh_data.end()) {
				material->AddMesh(it->second);
			} else {
				Mesh m;
				if(m.Initialize(raw_mesh) == false) {
					delete e;
					return false;
				}
				MeshData *meshdata = new MeshData(m);
				material->AddMesh(new MeshData(m));
				mesh_data.insert(std::pair<aiMesh *, MeshData *>(raw_mesh, meshdata));
			}
		}
		for(uint32_t i=0; i<node->mNumChildren; i++) {
			if(ProcessNode(node->mChildren[i], scene) == false) {
				return false;
			}
		}
		entities[name] = e;
	}

	return true;
}
}