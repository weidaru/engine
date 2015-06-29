#ifndef MODEL_H_	
#define MODEL_H_

#include "utils/s2string.h"
#include <stdint.h>

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

	
	const s2string & GetLastError() const;

	bool HasTextureCoordinates() const;
	
	uint32_t GetVertexNumber() const;
	//Return float[3]
	const float * GetPosition(uint32_t index) const;
	//Return float[3]
	const float * GetNormal(uint32_t index) const;
	
	uint32_t GetTextureCoordinateNumber() const;
	//Return float[2]
	const float * GetTextureCoordinate(uint32_t index, uint32_t texture_index) const;
	
	uint32_t GetTriangleSize() const;
	uint32_t GetTriangleVertexIndex(uint32_t index, uint32_t vertex_index) const;
	
private:
	void Clear();
	void Check() const;

private:

	
	s2string error;
};

}

#endif		MODEL_H_