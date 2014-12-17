#ifndef MODEL_H_	
#define MODEL_H_

#include "utils/s2string.h"

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
	
	unsigned int GetVertexSize() const;
	Vertex GetVertex(unsigned int index) const;
	
	unsigned int GetTriangleSize() const;
	unsigned int GetTriangleVertexIndex(unsigned int index, unsigned int vertex_index) const;
	
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