#ifndef MODEL_H_	
#define MODEL_H_

#include "utils/s2string.h"

namespace Assimp {

class aiMesh;

}

namespace s2 {

class Model {
public:
	struct Vertex {
		float x,y,z;
	};

public:
	Model();
	~Model();
	
	bool Read(const s2string &path);
	s2string GetLastError();
	
	unsigned int GetVertexSize() const;
	Vertex GetVertex(unsigned int index) const;
	
	unsigned int GetIndexSize() const;
	unsigned int GetIndex(unsigned int index) const;
	

private:
	aiMesh *mesh;
};

}

#endif		MODEL_H_