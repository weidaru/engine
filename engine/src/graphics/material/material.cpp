#include "material.h"

#include <glog/logging.h>

namespace s2 {

Material::Material(Entity *e) : Component(e), mesh(0) {
}

Material::~Material() {
}

Material & Material::SetMeshData(MeshData *m) {
	mesh = m;
	return *this;
}

MeshData * Material::GetMeshData() {
	return mesh;
}

}

