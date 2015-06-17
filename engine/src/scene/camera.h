#ifndef CAMERA_H_
#define CAMERA_H_

#include "utils/matrix4x4.h"
#include "utils/vector3.h"

#include "entity/entity.h"

namespace s2 {

/**
 * Assume column matrix and column vector
 * LH system, with default forward being [0.0f, 0.0f, 1.0f], up being [0.0f, 1.0f, 0.0f]
 */
class Camera : public Entity {
public:
	Camera(EntitySystem *system);
	
	void Reset();

	Matrix4x4 GetViewMatrix() const;
	
	//Up will be adjusted to forward to be orthogonal.
	Camera & SetDirectionVector(const Vector3 &forward, const Vector3 &up);
	const Vector3 & GetForwardVector() const { return forward; }
	const Vector3 & GetUpVector() const { return up; }

	Camera & TranslateForward(float distance);
	Camera & TranslateRight(float distance);

protected:
	virtual void OneFrame(float delta) override;

private:
	Camera(const Camera &);
	Camera & operator=(const Camera &);

private:
	Vector3 forward, up;
};

}

#endif		//CAMERA_H_
