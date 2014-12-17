#include "model.h"

#include <assimp/Importer.hpp>   
#include <assimp/scene.h> 
#include <assimp/postprocess.h>  
#include <glog/logging.h>


namespace s2 {

Model::Model() :  importer(0), mesh(0){

}

Model::~Model() {
	
}

void Model::Clear() {
	if(importer) {
		delete importer;
		importer = 0;
		mesh = 0;
	} else{
		delete mesh;
		mesh = 0;
	}
}

void Model::Check() {
	CHECK_NOTNULL(mesh)<<"Mesh is not initialized.";
}

bool Model::Initialize(const s2string &path) {
	Clear();
	
	importer = new Assimp::Importer;
	const aiScene* scene = importer.ReadFile( file_path, 
		aiProcess_CalcTangentSpace			| 
		aiProcess_Triangulate						|
		aiProcess_JoinIdenticalVertices		|
		aiProcess_FlipUVs 							|
		aiProcess_GenNormals 
		);
		
	if(scene == 0) {
		error = importer->GetErrorString();
	}
	//Use the first mesh.
	mesh = scene->mMeshes[0];
}

bool Model::Initialize(aiMesh *_mesh) {
	Clear();
	
	mesh = _mesh;
	
	return true;
}

const s2string & Model::GetLastError() {
	return error;
}

unsigned int Model::GetVertexSize() const {
	Check();
	return mesh->mNumVertices;
}

Model::Vertex Model::GetVertex(unsigned int index) const {
	Check();
	const aiFace& face = mesh->mFaces[i];
	Vertex result;

}

unsigned int Model::GetTriangleSize() const {
	Check();
	return mesh->mNumFaces;
}

unsigned int Model::GetTriangleIndex(unsigned int index, unsigned int vertex_index) const {
	Check();
	return mesh->mFaces[index].mIndices[vertex_index];
}

}

