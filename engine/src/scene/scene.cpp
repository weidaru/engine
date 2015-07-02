#include "scene.h"

#include "entity/entity.h"
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
	ProcessNode(scene->mRootNode, scene);
}

void Scene::ProcessNode(aiNode *node, const aiScene *scene) {
	if(node->mParent == 0 && node->mNumChildren != 0) {
		for(uint32_t i=0; i<node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	} else {
		s2string name = node->mName.C_Str();
		Entity *e = new Entity(Engine::GetSingleton()->GetEntitySystem());
		CHECK(entities.find(name)==entities.end()) << "entity "<<name<<" already exists.";
		entities[name] = e;

		Material *material = new Material(e);
		//Add to material system.
		for(uint32_t i=0; i<node->mNumMeshes; i++) {
			Mesh *m = new Mesh();
			m->Initialize(scene->mMeshes[node->mMeshes[i]]);
			material->AddMesh(m);
		}
	}

}


}