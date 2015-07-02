#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "entity/component.h"

#include <vector>

namespace s2 {

class Mesh;

class Material : public Component{
public:
	Material(Entity *e);
	virtual ~Material() override;

	uint32_t AddMesh(Mesh *m);
	uint32_t GetMeshSize();
	Mesh * GetMesh(uint32_t index);
	Mesh * RemoveMesh(uint32_t index);

private:
	std::vector<Mesh *> meshes;
};

}


#endif // !MATERIAL_H_
