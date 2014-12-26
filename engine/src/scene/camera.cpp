#include "camera.h"

#include <glog/logging.h>

#define PI 3.141596253f

namespace s2 {

namespace {

template <typename T>
T Clamp(T value, const T &floor, const T &ceiling) {
	T result = value<floor ? floor:value;
	return result>ceiling ? ceiling:result;
}

}

Camera::Camera() : calculate_vector(true), calculate_matrix(true) {
	Reset();
}

void Camera::Reset() {
	position.Set(0.0f, 0.0f, 0.0f);
	forward_raw.Set(0.0f, 0.0f, -1.0f);
	up_raw.Set(0.0f, 1.0f, 0.0f);
	alpha = 0.0f;
	beta = 0.0f;
	matrix.SetIdentity();
	calculate_vector = true;
	calculate_matrix = true;
}

void Camera::TurnUp(float angle) {
	alpha += angle;
	alpha = Clamp(alpha, -PI/2.0f, PI/2.0f);
	calculate_vector = true;
	calculate_matrix = true;
}

void Camera::TurnRight(float angle) {
	beta -= angle;
	calculate_vector = true;
	calculate_matrix = true;
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
	CalculateVectors();
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
	forward_raw = vec;
	forward_raw.Normalize();
	MakeVertical(&up_raw, forward_raw);
	calculate_matrix = true;
	calculate_vector = true;
	return *this;
}

Camera & Camera::SetUpVector(const Vector3 &vec) {
	up_raw = vec;
	up_raw.Normalize();
	MakeVertical(&up_raw, forward_raw);
	calculate_matrix = true;
	calculate_vector = true;
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
		temp.SetRotationX(alpha);
		m *= temp;
		temp.SetRotationY(beta);
		m *= temp;
		//No need to normalize as the matrix is rotation only.

		Vector3 zaxis = -1.0f * forward_raw;
		Vector3 xaxis = up_raw.Cross(zaxis);
		Vector3 yaxis = up_raw;
		Matrix4x4 basis;
		basis.Set(
			xaxis[0],  				yaxis[0], 				zaxis[0],			0.0f,
			xaxis[1],				yaxis[1], 				zaxis[1],			0.0f,
			xaxis[2], 				yaxis[2], 				zaxis[2],			0.0f,
			0.0f,						0.0f,						0.0f,					1.0f
		);

		//Intrinsic rotation
		m = basis * m; 
		
		forward = m*Vector4(0.0f, 0.0f, 1.0f, 0.0f);
		forward = -1.0f * forward;
		up = m*Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	}
	calculate_vector = false;
}

const Vector3 & Camera::GetPosition() {
	return position;
}

const Vector3 & Camera::GetForwardVector()  {
	CalculateVectors();
	return forward;
}

const Vector3 & Camera::GetUpVector() {
	CalculateVectors();
	return up;
}

}

