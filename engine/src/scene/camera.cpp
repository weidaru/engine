#include "camera.h"

#include <glog/logging.h>

namespace s2 {

Camera::Camera() : calculate_vector(true), calculate_matrix(true) {
	Reset();
}

void Camera::Reset() {
	position.Set(0.0f, 0.0f, 0.0f);
	forward_raw.Set(0.0f, 0.0f, -1.0f);
	up_raw.Set(0.0f, 1.0f, 0.0f);
	alpha = 0.0f;
	beta = 0.0f;
	gamma = 0.0f;
	matrix.SetIdentity();
	calculate_vector = true;
	calculate_matrix = true;
}

void Camera::TurnAroundLocalX(float angle) {
	alpha += angle;
	calculate_vector = true;
	calculate_matrix = true;
}

void Camera::TurnAroundLocalY(float angle) {
	beta += angle;
	calculate_vector = true;
	calculate_matrix = true;
}

void Camera::TurnAroundLocalZ(float angle) {
	gamma += angle;
	calculate_vector = true;
	calculate_matrix = true;
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
	forward_raw = vec;
	forward_raw.Normalize();
	MakeVertical(&up_raw, forward_raw);
	calculate_matrix = true;
	calculate_vector = false;
	return *this;
}

Camera & Camera::SetUpVector(const Vector3 &vec) {
	up_raw = vec;
	up_raw.Normalize();
	MakeVertical(&up_raw, forward_raw);
	calculate_matrix = true;
	calculate_vector = false;
	return *this;
}

const Matrix4x4 & Camera::GetViewMatrix() {
	CalculateVectors();

	if(calculate_matrix) {
		Vector3 zaxis = -1.0f * forward;
		Vector3 xaxis = up.Cross(zaxis);
		Vector3 yaxis = up;

		float p_dot_x = position.Dot(xaxis);
		float p_dot_y = position.Dot(yaxis);
		float p_dot_z = position.Dot(zaxis);
		
		matrix.Set(
			xaxis[0],  				xaxis[1], 				xaxis[2],			-p_dot_x,
			yaxis[0],				yaxis[1], 				yaxis[2],			-p_dot_y,
			zaxis[0], 				zaxis[1], 				zaxis[2],			-p_dot_z,
			0.0f,						0.0f,						0.0f,					1.0f
		);
	}
	calculate_matrix = false;
	
	return matrix;
}

void Camera::CalculateVectors() {
	if(calculate_vector) {
		Matrix4x4 m, temp;
		m.SetRotationX(alpha);
		temp.SetRotationY(beta);
		m *= temp;
		temp.SetRotationZ(gamma);
		m *= temp;
		//No need to normalize as the matrix is rotation only.
		
		forward = m*Vector4(forward_raw, 0.0f);
		up = m*Vector4(up_raw, 0.0f);
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

