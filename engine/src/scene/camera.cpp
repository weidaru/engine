#include "camera.h"
#include "entity/transform.h"

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

Camera::Camera(EntitySystem * system) : Entity(system) {
	Reset();
}

void Camera::Reset() {
	Transform &trans = *GetTransform();
	trans.ResetRotation();
	trans.ResetScaling();
	trans.ResetTranslate();

	forward.Set(0.0f, 0.0f, 1.0f);
	up.Set(0.0f, 1.0f, 0.0f);
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

Camera & Camera::SetDirectionVector(const Vector3 &_forward, const Vector3 &_up) {
	forward = _forward;
	forward.Normalize();
	up = _up;
	MakeVertical(&up, forward);

	return *this;
}

Camera & Camera::TranslateForward(float distance) {
	Transform &trans = *GetTransform();
	Vector3 real_forward = (trans.CalcualteRotateMatrix()*Vector4(forward, 0.0f)).ToVec3();
	trans.Translate(real_forward * distance);
}

Camera & Camera::TranslateRight(float distance) {
	Transform &trans = *GetTransform();
	Vector3 real_right = forward.Cross((trans.CalcualteRotateMatrix()*Vector4(up, 0.0f)).ToVec3());
	trans.Translate(real_right * distance);
}

Matrix4x4 Camera::GetViewMatrix() const {
	const Transform &trans = *GetTransform();

	Vector3 zaxis = -1.0f * (trans.CalcualteRotateMatrix()*Vector4(forward, 0.0f)).ToVec3();
	Vector3 yaxis = (trans.CalcualteRotateMatrix()*Vector4(up, 0.0f)).ToVec3();
	Vector3 xaxis = up.Cross(zaxis);

	Vector3 position = trans.GetTranslate();
	float p_dot_x = position.Dot(xaxis);
	float p_dot_y = position.Dot(yaxis);
	float p_dot_z = position.Dot(zaxis);
	
	return Matrix4x4(
		xaxis[0],  				xaxis[1], 				xaxis[2],			-p_dot_x,
		yaxis[0],				yaxis[1], 				yaxis[2],			-p_dot_y,
		zaxis[0], 				zaxis[1], 				zaxis[2],			-p_dot_z,
		0.0f,						0.0f,						0.0f,					1.0f
	);
}

}

