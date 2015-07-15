#ifndef MATERIAL_SYSTEM_H_
#define MATERIAL_SYSTEM_H_

#include "utils/s2vector3.h"
#include "utils/s2vector4.h"

#include <map>
#include <vector>

namespace s2 {

class Material;
class GraphicBuffer;
class VertexShader;
class PixelShader;
class Mesh;
class DrawingState;

//[[TypeInfo]]//
struct MaterialVertexData {
	S2Vector3 position;
	S2Vector3 normal;
	S2Vector4 color;
};

class MeshData {
public:
	MeshData(const Mesh &mesh);
	~MeshData();

	GraphicBuffer *GetVertexBuffer() { return vb; }
	GraphicBuffer *GetIndexBuffer() { return ib;}

private:
	GraphicBuffer *vb;
	GraphicBuffer *ib;
};

class MaterialSystem {
public:
	MaterialSystem();
	~MaterialSystem();

	void Register(Material *m);
	void Deregister(Material *m);

	void OneFrame(float delta);

	MeshData *CreateMeshdata();

private:
	std::vector<Material *> materials;

	VertexShader *vs;
	PixelShader *ps;

	DrawingState *drawing_state;
};


}

#endif // !MATERIAL_SYSTEM_H_
