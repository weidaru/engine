#include "material.h"

#include <glog/logging.h>

namespace s2 {

Material::Material(Entity *e) : Component(e) {
}

Material::~Material() {
}

uint32_t Material::AddMesh(MeshData *m) {
	meshes.push_back(m);
	return meshes.size()-1;
}

uint32_t Material::GetMeshSize() {
	return meshes.size();
}

MeshData * Material::GetMesh(uint32_t index) {
	return meshes[index];
}

MeshData * Material::RemoveMesh(uint32_t index) {
	MeshData *m = GetMesh(index);
	meshes.erase(meshes.begin() + index);
	return m;
}

}

