#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "entity/component.h"

#include <vector>

namespace s2 {

class MeshData;

class Material : public Component{
public:
	Material(Entity *e);
	virtual ~Material() override;

	uint32_t AddMesh(MeshData *m);
	uint32_t GetMeshSize();
	MeshData * GetMesh(uint32_t index);
	MeshData * RemoveMesh(uint32_t index);

private:
	std::vector<MeshData *> meshes;
};

}


#endif // !MATERIAL_H_
