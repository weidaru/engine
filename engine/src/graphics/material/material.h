#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "entity/component.h"

namespace s2 {

class Mesh;

class Material : public Component{
public:
	Material(Entity *e);
	virtual ~Material() override;

	Material & SetMesh(Mesh *m);
	Mesh *GetMesh();

private:
	Mesh *mesh;	
};

}


#endif // !MATERIAL_H_
