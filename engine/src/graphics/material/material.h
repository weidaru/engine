#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "entity/component.h"

namespace s2 {

class MeshData;

class Material : public Component{
public:
	Material(Entity *e);
	virtual ~Material() override;

	Material & SetMeshData(MeshData *m);
	MeshData * GetMeshData();

private:
	MeshData *mesh;
};

}


#endif // !MATERIAL_H_
