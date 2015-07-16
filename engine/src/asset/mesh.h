#ifndef MODEL_H_	
#define MODEL_H_

#include "utils/s2string.h"
#include <stdint.h>
#include "utils/s2vector3.h"
#include "utils/s2vector4.h"

namespace Assimp {
class Importer;
}
struct aiMesh;

namespace s2 {

class Mesh {
public:
	Mesh();
	~Mesh();
	
	bool Initialize(const s2string &path);
	bool Initialize(aiMesh *mesh);

	const s2string & GetLastError() const;

	uint32_t GetColorSetSize() const;
	S2Vector4 GetColor(uint32_t set_index, uint32_t vertex_index) const;
	
	uint32_t GetVertexSize() const;
	S2Vector3 GetPosition(uint32_t index) const;

	bool HasNormal() const;
	S2Vector3 GetNormal(uint32_t index) const;
	
	uint32_t GetTextureCoordinateSetSize() const;
	int32_t GetTextureCoordinateComponentSize(uint32_t set_index) const;
	S2Vector3 GetTextureCoordinate(uint32_t set_index, uint32_t texture_index) const;
	
	uint32_t GetTriangleSize() const;
	uint32_t GetTriangleVertexIndex(uint32_t triangle_index, uint32_t vertex_index) const;
	
private:
	void Check() const;

private:
	Assimp::Importer *importer;

	aiMesh *mesh;
	s2string error;
};

}

#endif		MODEL_H_