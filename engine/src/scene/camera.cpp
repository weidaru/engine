#include "camera.h"

#include <glog/logging.h>

namespace s2 {

Camera::Camera() : calculate_vector(true), calculate_matrix(true) {
	Reset();
}

void Camera::Reset() {
	position.Set(0.0f, 0.0f, 0.0f);
	forward.Set(0.0f, 0.0f, -1.0f);
	up.Set(0.0f, 1.0f, 0.0f);
	matrix.SetIdentity();
	calculate_vector = false;
	calculate_matrix = false;
}

void Camera::TurnAroundLocalX(float angel) {
	Matrix4x4 rot;
	rot.SetRotationX(angel);
	//Intrinsic rotation in column matrix
	GetViewMatrix();
	matrix *= rot;
	calculate_vector = true;
}

void Camera::TurnAroundLocalY(float angel) {
	Matrix4x4 rot;
	rot.SetRotationY(angel);
	//Intrinsic rotation in column matrix
	GetViewMatrix();
	matrix *= rot;
	calculate_vector = true;
}

void Camera::TurnAroundLocalZ(float angel) {
	Matrix4x4 rot;
	rot.SetRotationZ(angel);
	//Intrinsic rotation in column matrix
	GetViewMatrix();
	matrix *= rot;
	calculate_vector = true;
}

Camera & Camera::Move(const Vector3 &movement) {
	position += movement;
	calculate_matrix = true;
	return *this;
}

Camera & Camera::SetPosition(const Vector3 &vec) {
	position = vec;
	calculate_matrix = true;
	return *this;
}

namespace {

/**
 * Make lhs vertical to rhs and still in the same plane.
 */
void MakeVertical(Vector3 *_lhs, const Vector3 &rhs) {
	Vector3 &lhs = *_lhs;
	float c = lhs.Dot(rhs);
	CHECK(c!=1.0f)<<"Input cannot be in the same line.";
	lhs -= (rhs*c);
	lhs.Normalize();
}

}

Camera & Camera::SetForwardVector(const Vector3 &vec) {
	forward = vec;
	forward.Normalize();
	MakeVertical(&up, forward);
	calculate_matrix = true;
	calculate_vector = false;
	return *this;
}

Camera & Camera::SetUpVector(const Vector3 &vec) {
	up = vec;
	up.Normalize();
	MakeVertical(&up, forward);
	calculate_matrix = true;
	calculate_vector = false;
	return *this;
}

const Matrix4x4 & Camera::GetViewMatrix() {
	if(calculate_matrix) {
		Vector3 right = forward.Cross(up);
		float p_dot_r = position.Dot(right);
		float p_dot_u = position.Dot(up);
		float p_dot_f = position.Dot(forward);
		
		matrix.Set(
			right[0],  	up[0], 		forward[0],	0.0f,
			right[1],	up[1], 		forward[1],	0.0f,
			right[2], 	up[2], 		forward[2],	0.0f,
			p_dot_r,	p_dot_u,	p_dot_f,		0.0f
		);
	}
	calculate_matrix = false;
	
	return matrix;
}

void Camera::CalculateVectors() {
	if(calculate_vector) {
		//No need to normalize as the matrix is rotation only.
		forward = matrix*Vector4(0.0f, 0.0f, -1.0f, 0.0f);
		up = matrix*Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	}
	calculate_vector = false;
}

const Vector3 & Camera::GetPosition() {
	return position;
}

const Vector3 & Camera::GetForwardVector()  {
	CalculateVectors();
	return position;
}

const Vector3 & Camera::GetUpVector() {
	CalculateVectors();
	return up;
}

}

