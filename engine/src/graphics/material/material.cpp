#include "material.h"

#include <glog/logging.h>

namespace s2 {

Material::Material(Entity *e) : Component(e) {
}

Material::~Material() {
}

uint32_t Material::AddMesh(Mesh *m) {
	meshes.push_back(m);
}

uint32_t Material::GetMeshSize() {
	return meshes.size();
}

Mesh * Material::GetMesh(uint32_t index) {
	return meshes[index];
}

Mesh * Material::RemoveMesh(uint32_t index) {
	Mesh *m = GetMesh(index);
	meshes.erase(meshes.begin() + index);
}

}

