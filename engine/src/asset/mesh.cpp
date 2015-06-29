#include "mesh.h"

#include <assimp/Importer.hpp>   
#include <assimp/scene.h> 
#include <assimp/postprocess.h>  
#include <glog/logging.h>


namespace s2 {

Mesh::Mesh() :  importer(0), mesh(0){

}

Mesh::~Mesh() {
	Clear();
}

void Mesh::Clear() {
	if(importer) {
		delete importer;
		importer = 0;
		mesh = 0;
	} else{
		delete mesh;
		mesh = 0;
	}
}

void Mesh::Check() const {
	CHECK_NOTNULL(mesh);
}

bool Mesh::Initialize(const s2string &path) {
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

const s2string & Mesh::GetLastError() const {
	return error;
}

uint32_t Mesh::GetVertexSize() const {
	Check();
	return mesh->mNumVertices;
}

bool Mesh::HasTextureCoordinates() const {
	Check();
	return mesh->HasTextureCoords(0);
}

Mesh::Vertex Mesh::GetVertex(uint32_t index) const {
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

uint32_t Mesh::GetTriangleSize() const {
	Check();
	return mesh->mNumFaces;
}

uint32_t Mesh::GetTriangleVertexIndex(uint32_t index, uint32_t vertex_index) const {
	Check();
	return mesh->mFaces[index].mIndices[vertex_index];
}

}

