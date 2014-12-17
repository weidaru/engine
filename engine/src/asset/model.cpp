#include "model.h"

#include <assimp/Importer.hpp>   
#include <assimp/scene.h> 
#include <assimp/postprocess.h>  
#include <glog/logging.h>


namespace s2 {

Model::Model() :  importer(0), mesh(0){

}

Model::~Model() {
	Clear();
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

void Model::Check() const {
	CHECK_NOTNULL(mesh);
}

bool Model::Initialize(const s2string &path) {
	Clear();
	
	importer = new Assimp::Importer;
	const aiScene* scene = importer->ReadFile( path, 
		aiProcess_CalcTangentSpace			| 
		aiProcess_Triangulate						|
		aiProcess_JoinIdenticalVertices		|
		aiProcess_FlipUVs 						|
		aiProcess_GenNormals
		);
		
	if(scene == 0) {
		error = importer->GetErrorString();
		Clear();
		return false;
	}
	//Use the first mesh.
	mesh = scene->mMeshes[0];

	return true;
}

bool Model::Initialize(aiMesh *_mesh) {
	Clear();
	
	mesh = _mesh;
	
	if(mesh) {
		return true;
	} else {
		return false;
	}
}

const s2string & Model::GetLastError() const {
	return error;
}

unsigned int Model::GetVertexSize() const {
	Check();
	return mesh->mNumVertices;
}

bool Model::HasTextureCoordinates() const {
	Check();
	return mesh->HasTextureCoords(0);
}

Model::Vertex Model::GetVertex(unsigned int index) const {
	Check();
	Vertex result;
	
	result.x =  mesh->mVertices[index].x;
	result.y =  mesh->mVertices[index].y;
	result.z =  mesh->mVertices[index].z;
	result.nx = mesh->mNormals[index].x;
	result.ny = mesh->mNormals[index].y;
	result.nz = mesh->mNormals[index].z;
	if(mesh->HasTextureCoords(0)) {
		result.u = mesh->mTextureCoords[0][index].x;
		result.v = mesh->mTextureCoords[0][index].y;
	}
	
	return result;
}

unsigned int Model::GetTriangleSize() const {
	Check();
	return mesh->mNumFaces;
}

unsigned int Model::GetTriangleVertexIndex(unsigned int index, unsigned int vertex_index) const {
	Check();
	return mesh->mFaces[index].mIndices[vertex_index];
}

}

