#include "camera.h"
#include "entity/transform.h"
#include "engine.h"
#include "input/input_system.h"

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
}



namespace {

/**
 * Make lhs vertical to rhs and still in the same plane.
 */
void MakeVertical(S2Vector3 *_lhs, const S2Vector3 &rhs) {
	S2Vector3 &lhs = *_lhs;
	float c = lhs.Dot(rhs);
	CHECK(c!=1.0f)<<"Input cannot be in the same line.";
	lhs -= (rhs*c);
	lhs.Normalize();
}

}

Camera & Camera::SetForwardVector(const S2Vector3 &_forward) {
	forward = _forward;
	forward.Normalize();
	return *this;
}

Camera & Camera::TranslateForward(float distance) {
	Transform &trans = *GetTransform();
	//Use spherical coordinate for forward vector.
	const S2Vector3 & rotate = trans.GetRotate();
	float theta = rotate[0], phi = rotate[1];
	phi = phi > 1.57f ? 1.57f : phi;
	phi = phi < -1.57f ? -1.57f : phi;

	S2Vector3 zaxis(sin(theta)*cos(phi), sin(phi), cos(theta)*cos(phi));

	trans.Translate(zaxis * distance);

	return *this;
}

Camera & Camera::TranslateRight(float distance) {
	Transform &trans = *GetTransform();
	//Use spherical coordinate for forward vector.
	const S2Vector3 & rotate = trans.GetRotate();
	float theta = rotate[0], phi = rotate[1];
	phi = phi > 1.57f ? 1.57f : phi;
	phi = phi < -1.57f ? -1.57f : phi;

	S2Vector3 zaxis(sin(theta)*cos(phi), sin(phi), cos(theta)*cos(phi));
	S2Vector3 xaxis = S2Vector3(0.0f, 1.0f, 0.0f).Cross(zaxis);

	trans.Translate(xaxis * distance);

	return *this;
}

S2Matrix4x4 Camera::GetViewMatrix() const {
	const Transform &trans = *GetTransform();

	//Use spherical coordinate for forward vector.
	const S2Vector3 & rotate = trans.GetRotate();
	float theta = rotate[0], phi = rotate[1];
	phi = phi > 1.57f ? 1.57f : phi;
	phi = phi < -1.57f ? -1.57f : phi;

	S2Vector3 zaxis(sin(theta)*cos(phi), sin(phi), cos(theta)*cos(phi));
	S2Vector3 xaxis = S2Vector3(0.0f, 1.0f, 0.0f).Cross(zaxis);
	xaxis.Normalize();
	S2Vector3 yaxis = zaxis.Cross(xaxis);

	S2Vector3 position = trans.GetTranslate();
	float p_dot_x = position.Dot(xaxis);
	float p_dot_y = position.Dot(yaxis);
	float p_dot_z = position.Dot(zaxis);
	
	return S2Matrix4x4(
		xaxis[0],  				xaxis[1], 				xaxis[2],			-p_dot_x,
		yaxis[0],				yaxis[1], 				yaxis[2],			-p_dot_y,
		zaxis[0], 				zaxis[1], 				zaxis[2],			-p_dot_z,
		0.0f,						0.0f,						0.0f,					1.0f
	);
}

void Camera::OneFrame(float delta) {

	InputSystem &is = *Engine::GetSingleton()->GetInputSystem();
	int delta_x = is.GetMouseXMove();
	int delta_y = is.GetMouseYMove();
		
	GetTransform()->Rotate(delta_x/1000.0f,  -delta_y/1000.0f, 0.0f);
	
	float mouse_x_p = is.GetMouseXPercent(), mouse_y_p = is.GetMouseYPercent();
	if(mouse_x_p<0.4f || mouse_x_p>0.6f || mouse_y_p<0.4f || mouse_y_p>0.6f ) {
		is.SetMousePositionPercent(0.5f, 0.5f);
	}

	if(is.IsKeyDown(InputSystem::K_W)) {
		TranslateForward(20.0f*delta);
	} else if(is.IsKeyDown(InputSystem::K_S)) {
		TranslateForward(-20.0f*delta);
	}
	if(is.IsKeyDown(InputSystem::K_D)) {
		TranslateRight(20.0f*delta);
	} else if(is.IsKeyDown(InputSystem::K_A)) {
		TranslateRight(-20.0f*delta);
	}
}

}

