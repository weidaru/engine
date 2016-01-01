#ifndef CAMERA_H_
#define CAMERA_H_

#include "utils/s2matrix4x4.h"
#include "utils/s2vector3.h"

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

	S2Matrix4x4 GetViewMatrix() const;
	
	//Up will be adjusted to forward to be orthogonal.
	Camera & SetForwardVector(const S2Vector3 &forward);
	const S2Vector3 & GetForwardVector() const { return forward; }

	Camera & TranslateForward(float distance);
	Camera & TranslateRight(float distance);

	Camera & SetMouseReset(bool new_value) { mouse_reset = new_value; }
	bool GetMouseReset() { return mouse_reset; }

protected:
	virtual void OneFrame(float delta) override;

private:
	Camera(const Camera &);
	Camera & operator=(const Camera &);

private:
	S2Vector3 forward;
	bool mouse_reset;
};

}

#endif		//CAMERA_H_
