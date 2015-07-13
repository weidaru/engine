#ifndef MATERIAL_SYSTEM_H_
#define MATERIAL_SYSTEM_H_

#include <utils/vector3.h>
#include <utils/vector4.h>

#include <map>
#include <vector>

namespace s2 {

class Material;
class Buffer;
class VertexShader;
class PixelShader;
class Mesh;

//[[TypeInfo]]//
struct MaterialVertexData {
	Vector3 position;
	Vector3 normal;
	Vector4 color;
};

class MeshData {
public:
	MeshData(const Mesh &mesh);
	~MeshData();

	Buffer *GetVertexBuffer() { return vb; }
	Buffer *GetIndexBuffer() { return ib;}

private:
	Buffer *vb;
	Buffer *ib;
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
};


}

#endif // !MATERIAL_SYSTEM_H_
