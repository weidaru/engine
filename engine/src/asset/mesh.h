#ifndef MODEL_H_	
#define MODEL_H_

#include "utils/s2string.h"
#include <stdint.h>
#include "utils/vector3.h"

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
	
	uint32_t GetVertexSize() const;
	Vector3 GetPosition(uint32_t index) const;
	Vector3 GetNormal(uint32_t index) const;
	
	uint32_t GetTextureCoordinateSize() const;
	int32_t GetTextureCoordinateComponentSize(uint32_t index) const;
	Vector3 GetTextureCoordinate(uint32_t index, uint32_t texture_index) const;
	
	uint32_t GetTriangleSize() const;
	uint32_t GetTriangleVertexIndex(uint32_t index, uint32_t vertex_index) const;
	
private:
	void Check() const;

private:
	Assimp::Importer *importer;

	aiMesh *mesh;
	s2string error;
};

}

#endif		MODEL_H_