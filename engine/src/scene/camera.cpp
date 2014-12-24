#include "camera.h"

#include <glog/logging.h>

#define PI 3.141596253f

namespace s2 {

Camera::Camera() : calculate_matrix(true) {
	Reset();
}

void Camera::Reset() {
	position.Set(0.0f, 0.0f, 0.0f);
	forward.Set(0.0f, 0.0f, -1.0f);
	up.Set(0.0f, 1.0f, 0.0f);
	rotation.SetIdentity();
	matrix.SetIdentity();
	calculate_matrix = false;
}

void Camera::TurnAroundLocalX(float angle) {
	Matrix4x4 temp;
	temp.SetRotationX(-angle);
	rotation = temp * rotation;
	calculate_matrix = true;
}

void Camera::TurnAroundLocalY(float angle) {
	Matrix4x4 temp;
	temp.SetRotationY(-angle);
	rotation = temp * rotation;
	calculate_matrix = true;
}

void Camera::TurnAroundLocalZ(float angle) {
	Matrix4x4 temp;
	temp.SetRotationZ(-angle);
	rotation = temp * rotation;
	calculate_matrix =true;
}

Camera & Camera::Move(const Vector3 &movement) {
	position += movement;
	calculate_matrix = true;
	return *this;
}

Camera & Camera::MoveForward(float distance) {
	return Move(GetForwardVector()*distance);
}

Camera & Camera::MoveRight(float distance){
	Vector3 right = forward.Cross(up);

	return Move(right*distance);
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
	CalculateRotationMatrix();
	calculate_matrix = true;
	return *this;
}

Camera & Camera::SetUpVector(const Vector3 &vec) {
	up = vec;
	up.Normalize();
	MakeVertical(&up, forward);
	CalculateRotationMatrix();
	calculate_matrix = true;
	return *this;
}

void Camera::CalculateRotationMatrix() {
	Vector3 zaxis = -1.0f * forward;
	Vector3 xaxis = up.Cross(zaxis);
	Vector3 yaxis = up;
	
	rotation.Set(
		xaxis[0],  				xaxis[1], 				xaxis[2],			0.0f,
		yaxis[0],				yaxis[1], 				yaxis[2],			0.0f,
		zaxis[0], 				zaxis[1], 				zaxis[2],			0.0f,
		0.0f,						0.0f,						0.0f,					1.0f
	);
}

const Matrix4x4 & Camera::GetViewMatrix() {
	if(calculate_matrix) {
		Matrix4x4 translation;
		translation[0][3] = -position[0];
		translation[1][3] = -position[1];
		translation[2][3] = -position[2];
		
		matrix = rotation*translation;
	}
	calculate_matrix = false;
	
	return matrix;
}

const Vector3 & Camera::GetPosition() {
	return position;
}

const Vector3 & Camera::GetForwardVector()  {
	return forward;
}

const Vector3 & Camera::GetUpVector() {
	return up;
}

}

