#include "transform.h"

namespace s2 {

Transform::Transform(Entity *entity) : Component(entity), needCompute(false) {

}

const Matrix4x4 & Transform::GetMatrix() const {
	if(!needCompute) {
		return matrix;
	}

	Matrix4x4 scale_matrix, rotation_matrix;
	scale_matrix.SetScale(scale);

	//Use XYZ intrinsic rotation.
	Matrix4x4 temp;
	rotation_matrix.SetRotationX(rotate[0]);
	temp.SetRotationY(rotate[1]);
	rotation_matrix *= temp;
	temp.SetRotationZ(rotate[2]);
	rotation_matrix *= temp;

	//Scale  ->  Rotate  ->  Translate
	matrix.SetTranslate(translate);
	matrix *= rotation_matrix;
	matrix *= scale_matrix;

	needCompute = false;

	return matrix;
}

Transform & Transform::Translate(float x, float y, float z) {
	translate[0] += x;
	translate[1] += y;
	translate[2] += z;
	needCompute = true;
	return *this;
}

Transform & Transform::Translate(const Vector3 &vector) {
	translate += vector;
	needCompute = true;
	
	return *this;
}

Transform & Transform::SetTranslate(float x, float y, float z) {
	translate[0] = x;
	translate[1] = y;
	translate[2] = z;
	needCompute = true;
	
	return *this;
}

Transform & Transform::SetTranslate(const Vector3 &vector) {
	translate = vector;
	needCompute = true;

	return *this;
}

Transform & Transform::ResetTranslate() {
	translate.Set(0.0f, 0.0f, 0.0f );
	needCompute = true;
	return *this;
}

Transform & Transform::Scale(float x, float y, float z) {
	scale[0] += x;
	scale[1] += y;
	scale[2] += z;
	needCompute = true;
	return *this;
}

Transform & Transform::Scale(const Vector3 &vector) {
	return Scale(vector[0], vector[1], vector[2]);
}

Transform & Transform::SetScale(float x, float y, float z) {
	scale[0] = x;
	scale[1] = y;
	scale[2] = z;
	needCompute = true;
	return *this;
}

Transform & Transform::SetScale(const Vector3 &vector) {
	return SetScale(vector[0], vector[1], vector[2]);
}

Transform & Transform::ResetScaling() {
	scale.Set(0.0f, 0.0f, 0.0f);
	needCompute = true;
	return *this;
}


Transform & Transform::Rotate(float x, float y ,float z) {
	rotate[0] += x;
	rotate[1] += y;
	rotate[2] += z;
	needCompute = true;
	return *this;
}

Transform & Transform::Rotate(const Vector3 &vector) {
	return Rotate(vector[0], vector[1], vector[2]);
}

Transform & Transform::SetRotate(float x, float y, float z) {
	rotate[0] = x;
	rotate[1] = y;
	rotate[2] = z;
	needCompute = true;
	return *this;
}

Transform & Transform::SetRotate(const Vector3 &vector) {
	return SetRotate(vector[0], vector[1], vector[2]);
}

Transform & Transform::ResetRotation() {
	rotate.Set(0.0f, 0.0f, 0.0f);
	needCompute = true;
	return *this;
}

}


