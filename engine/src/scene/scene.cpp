#include "scene.h"

#include "entity/entity.h"
#include "entity/entity_system.h"
#include "entity/transform.h"
#include "engine.h"

#include "graphics/material/material.h"
#include "graphics/material/material_system.h"

#include "asset/mesh.h"

#include <assimp/Importer.hpp>   
#include <assimp/scene.h> 
#include <assimp/postprocess.h>  

#include <glog/logging.h>

namespace s2 {

Scene::Scene(EntitySystem *_entity_system) : entity_system(_entity_system), importer(0) {
	CHECK_NOTNULL(entity_system);
}

Scene::~Scene() {
	for(auto it=entities.begin(); it!=entities.end(); it++) {
		delete entity_system->Remove(it->second->GetId());
	}
	
	for(auto it=mesh_data.begin(); it!=mesh_data.end(); it++) {
		delete it->second;
	}

	delete importer;
}

bool Scene::Initialize(const s2string &path) {
	importer = new Assimp::Importer;

	if(path.substr(path.size()-4, 4) != ".fbx") {
		error = "Only support .fbx scene file.";
		return false;
	}

	const aiScene* scene = importer->ReadFile( path, 
		aiProcess_CalcTangentSpace		| 
		aiProcess_Triangulate				|
		aiProcess_JoinIdenticalVertices	|
		aiProcess_FlipUVs 						|
		aiProcess_GenSmoothNormals	|
		aiProcess_MakeLeftHanded 
		);

	if(scene == 0) {
		error = importer->GetErrorString();
		return false;
	}
	if(scene->mRootNode == 0) {
		error = "No root node for scene " + path;
		return false;
	}

	return ProcessNode(scene->mRootNode, 0, scene) != 0;
}

Entity * Scene::ProcessNode(aiNode *node, Entity * parent_entity, const aiScene *scene) {
	if(node->mParent == 0) {
		CHECK(parent_entity==0)<<"parent_entity must be 0 for root node.";

		for(uint32_t i=0; i<node->mNumChildren; i++) {
			s2string child_name = node->mChildren[i]->mName.C_Str();
			CHECK(entities.find(child_name)==entities.end()) << "entity "<<child_name<<" already exists.";
			Entity *child_entity =ProcessNode(node->mChildren[i], 0, scene);
			if(child_entity) {
				entities[child_name] = child_entity;
			} else {
				return 0;
			}
		}

		return (Entity *)1;			//Anything except for 0 should be OK.
	}

	s2string name = node->mName.C_Str();
	if(node->mNumMeshes > 1) {
		S2StringFormat(&error, "Node %s has more than 1 meshes which is not allowed.", name.c_str());
		return 0;
	}
	CHECK(entities.find(name)==entities.end()) << "entity "<<name<<" already exists.";

	Entity *entity = new Entity(entity_system);

	if(node->mNumMeshes==1 ) {
		Material *material = new Material(entity);
		Engine::GetSingleton()->GetMaterialSystem()->Register(material);
		aiMesh *raw_mesh = scene->mMeshes[node->mMeshes[0]];
		auto it = mesh_data.find(raw_mesh);
		if(it != mesh_data.end()) {
			material->SetMeshData(it->second);
		} else {
			Mesh m;
			if(m.Initialize(raw_mesh) == false) {
				delete entity_system->Remove(entity->GetId());
				return false;
			}
			MeshData *meshdata = new MeshData(m);
			material->SetMeshData(new MeshData(m));
			mesh_data.insert(std::pair<aiMesh *, MeshData *>(raw_mesh, meshdata));
		}
	}

	//Set transform.
	Transform *transform = entity->GetTransform();
	aiQuaternion rotation;
	aiVector3D translate, scaling;
	node->mTransformation.Decompose(scaling, rotation, translate);
	transform->SetScale(scaling.x, scaling.y, scaling.z);
	transform->SetTranslate(translate.x, translate.y, translate.z);

	S2Vector3 rotation_euler;
	float q0=rotation.x, q1=rotation.y, q2=rotation.z, q3=rotation.w;
	rotation_euler[2] = atan(2*(q0*q1+q2*q3)/(1-2*(q1*q1+q2*q2)));
	rotation_euler[1] = asin(2*(q0*q2-q3*q1));
	rotation_euler[0] = atan(2*(q0*q3+q1*q2)/(1-2*(q2*q2+q3*q3)));

	transform->SetRotate(rotation_euler);
	
	for(uint32_t i=0; i<node->mNumChildren; i++) {
		if(ProcessNode(node->mChildren[i], entity, scene) == false) {
			delete entity_system->Remove(entity->GetId());
			return false;
		}
	}

	if(parent_entity) {
		parent_entity->AddChild(entity);
	}

	return entity;
}
}