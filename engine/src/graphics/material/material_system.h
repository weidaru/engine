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
class InputLayout;
class PixelShader;
class Mesh;
class GraphicPipelineState;
class GraphicResourceManager;
class ShaderData;

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
	MaterialSystem(GraphicResourceManager *_manager);
	~MaterialSystem();

	void Register(Material *m);
	void Deregister(Material *m);

	void OneFrame(float delta);

	MeshData *CreateMeshdata();

private:
	GraphicResourceManager *manager;

	std::vector<Material *> materials;

	VertexShader *vs;
	PixelShader *ps;
	ShaderData *vs_data, *ps_data;
	InputLayout *input_layout;

	GraphicPipelineState *pipeline_state;
};


}

#endif // !MATERIAL_SYSTEM_H_
