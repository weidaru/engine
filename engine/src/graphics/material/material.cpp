#include "material.h"

#include <glog/logging.h>

namespace s2 {

Material::Material(Entity *e) : Component(e) {
}

Material::~Material() {
}

Material & Material::SetMesh(Mesh *m) {
	mesh = m; return *this; 
}

Mesh *Material::GetMesh() {
	return mesh; 
}

}

