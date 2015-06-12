#ifndef MODEL_H_	
#define MODEL_H_

#include "utils/s2string.h"
#include <stdint.h>

namespace Assimp {
class Importer;
}

struct aiMesh;

namespace s2 {

class Model {
public:
	struct Vertex {
		float x,y,z;
		float nx,ny,nz;
		float u,v;
	};

public:
	Model();
	~Model();
	
	bool Initialize(const s2string &path);
	bool Initialize(aiMesh *_mesh);
	
	const s2string & GetLastError() const;

	bool HasTextureCoordinates() const;
	
	uint32_t GetVertexSize() const;
	Vertex GetVertex(uint32_t index) const;
	
	uint32_t GetTriangleSize() const;
	uint32_t GetTriangleVertexIndex(uint32_t index, uint32_t vertex_index) const;
	
private:
	void Clear();
	void Check() const;

private:
	Assimp::Importer *importer;
	aiMesh *mesh;
	
	s2string error;
};

}

#endif		MODEL_H_