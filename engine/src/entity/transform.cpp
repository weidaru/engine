#include "transform.h"

namespace s2 {

Transform::Transform(Entity *entity) : Component(entity), need_compute(false), scale(1.0f, 1.0f, 1.0f) {

}

const S2Matrix4x4 & Transform::GetPrefixMatrix() const {
	return prefix_matrix;
}

Transform & Transform::SetPrefixMatrix(const S2Matrix4x4 &m) {
	prefix_matrix = m;
	need_compute = true;
	return *this;
}

const S2Matrix4x4 & Transform::GetSuffixMatrix() const {
	return suffix_matrix;
}

Transform & Transform::SetSuffixMatrix(const S2Matrix4x4 &m) {
	suffix_matrix = m;
	need_compute = true;
	return *this;
}

const S2Matrix4x4 & Transform::GetMatrix() const {
	if(!need_compute) {
		return matrix;
	}

	S2Matrix4x4 scale_matrix = CalculateScaleMatrix(), rotation_matrix = CalcualteRotateMatrix();

	//prefix -> Scale  ->  Rotate  ->  Translate -> suffix
	matrix = suffix_matrix;
	matrix *= CalculateTranslateMatrix();
	matrix *= rotation_matrix;
	matrix *= scale_matrix;

	matrix *= prefix_matrix;

	need_compute = false;
	return matrix;
}

S2Matrix4x4 Transform::CalculateTranslateMatrix() const {
	S2Matrix4x4 result;
	return result.SetTranslate(translate);
}


S2Matrix4x4 Transform::CalculateScaleMatrix() const {
	S2Matrix4x4 result;
	return result.SetScale(scale);
}

//XYZ rotation.
S2Matrix4x4 Transform::CalcualteRotateMatrix() const {
	S2Matrix4x4 temp, result;
	result.SetRotationZ(rotate[2]);
	temp.SetRotationY(rotate[1]);
	result *= temp;
	temp.SetRotationX(rotate[0]);
	return result *= temp;
}

Transform & Transform::Translate(float x, float y, float z) {
	translate[0] += x;
	translate[1] += y;
	translate[2] += z;
	need_compute = true;
	return *this;
}

Transform & Transform::Translate(const S2Vector3 &vector) {
	translate += vector;
	need_compute = true;
	
	return *this;
}

Transform & Transform::SetTranslate(float x, float y, float z) {
	translate[0] = x;
	translate[1] = y;
	translate[2] = z;
	need_compute = true;
	
	return *this;
}

Transform & Transform::SetTranslate(const S2Vector3 &vector) {
	translate = vector;
	need_compute = true;

	return *this;
}

Transform & Transform::ResetTranslate() {
	translate.Set(0.0f, 0.0f, 0.0f );
	need_compute = true;
	return *this;
}

Transform & Transform::Scale(float x, float y, float z) {
	scale[0] += x;
	scale[1] += y;
	scale[2] += z;
	need_compute = true;
	return *this;
}

Transform & Transform::Scale(const S2Vector3 &vector) {
	return Scale(vector[0], vector[1], vector[2]);
}

Transform & Transform::SetScale(float x, float y, float z) {
	scale[0] = x;
	scale[1] = y;
	scale[2] = z;
	need_compute = true;
	return *this;
}

Transform & Transform::SetScale(const S2Vector3 &vector) {
	return SetScale(vector[0], vector[1], vector[2]);
}

Transform & Transform::ResetScaling() {
	scale.Set(0.0f, 0.0f, 0.0f);
	need_compute = true;
	return *this;
}


Transform & Transform::Rotate(float x, float y ,float z) {
	rotate[0] += x;
	rotate[1] += y;
	rotate[2] += z;
	need_compute = true;
	return *this;
}

Transform & Transform::Rotate(const S2Vector3 &vector) {
	return Rotate(vector[0], vector[1], vector[2]);
}

Transform & Transform::SetRotate(float x, float y, float z) {
	rotate[0] = x;
	rotate[1] = y;
	rotate[2] = z;
	need_compute = true;
	return *this;
}

Transform & Transform::SetRotate(const S2Vector3 &vector) {
	return SetRotate(vector[0], vector[1], vector[2]);
}

Transform & Transform::ResetRotation() {
	rotate.Set(0.0f, 0.0f, 0.0f);
	need_compute = true;
	return *this;
}
}


