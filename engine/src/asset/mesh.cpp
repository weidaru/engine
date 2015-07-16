#include "mesh.h"

#include <assimp/Importer.hpp>   
#include <assimp/scene.h> 
#include <assimp/postprocess.h>  
#include <glog/logging.h>


namespace s2 {

Mesh::Mesh() : importer(0), mesh(0) {

}

Mesh::~Mesh() {
	delete importer;
}

bool Mesh::Initialize(const s2string &path) {
	importer = new Assimp::Importer;
	const aiScene* scene = importer->ReadFile( path, 
		aiProcess_CalcTangentSpace		| 
		aiProcess_Triangulate				|
		aiProcess_JoinIdenticalVertices	|
		aiProcess_FlipUVs 						|
		aiProcess_GenSmoothNormals	|
		aiProcess_MakeLeftHanded 
		);

	if(scene == 0) {
		error = importer->GetErrorString();
		return false;
	}
	if(scene->HasMeshes()) {
		return Initialize(scene->mMeshes[0]);
	} else {
		error = "file does not contain a mesh";
		return false;
	}
}

bool Mesh::Initialize(aiMesh *_mesh) {
	if(_mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE ) {
		error = "Input mesh must triangulated.";
		return false;
	}

	mesh = _mesh;

	return true;
}

const s2string & Mesh::GetLastError() const {
	return error;
}

uint32_t Mesh::GetVertexSize() const {
	Check();
	return mesh->mNumVertices;
}

namespace {

S2Vector3 ConvertVector(const aiVector3D &origin) {
	return S2Vector3(origin.x, origin.y, origin.z);
}

}

uint32_t Mesh::GetColorSetSize() const {
	Check();
	return mesh->GetNumColorChannels();
}

S2Vector4 Mesh::GetColor(uint32_t set_index, uint32_t vertex_index) const {
	const aiColor4D &color = mesh->mColors[set_index][vertex_index];

	return S2Vector4(color.r, color.g, color.b, color.a);
}

S2Vector3 Mesh::GetPosition(uint32_t index) const {
	Check();
	return ConvertVector(mesh->mVertices[index]);
}

bool Mesh::HasNormal() const {
	Check();
	return mesh->HasNormals();
}

S2Vector3 Mesh::GetNormal(uint32_t index) const {
	Check();
	CHECK(HasNormal())<<"Do not have normal.";
	return ConvertVector(mesh->mNormals[index]);
}

uint32_t Mesh::GetTextureCoordinateSetSize() const {
	Check();
	return mesh->GetNumUVChannels();
}

int32_t Mesh::GetTextureCoordinateComponentSize(uint32_t set_index) const {
	Check();
	return mesh->mNumUVComponents[set_index];
}

S2Vector3 Mesh::GetTextureCoordinate(uint32_t set_index, uint32_t vertex_index) const {
	Check();
	return ConvertVector(mesh->mTextureCoords[set_index][vertex_index]);
}

uint32_t Mesh::GetTriangleSize() const {
	Check();
	return mesh->mNumFaces;
}

uint32_t Mesh::GetTriangleVertexIndex(uint32_t triangle_index, uint32_t vertex_index) const {
	Check();
	return mesh->mFaces[triangle_index].mIndices[vertex_index];
}

void Mesh::Check() const {
	CHECK_NOTNULL(mesh);
}

}

