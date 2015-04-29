#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include "component.h"
#include "utils/matrix4x4.h"
#include "utils/vector3.h"

namespace s2 {

class Transform : Component {
public:
	Transform();

	const Matrix4x4 & GetMatrix() const;

	Transform & Translate(float x, float y, float z);
	Transform & Translate(const Vector3 &vector);
	Transform & SetTranslate(float x, float y, float z);
	Transform & SetTranslate(const Vector3 &vectro);
	const Vector3 & GetTranslate() { return translate; }
	Transform & ResetTranslate();

	Transform & Scale(float x, float y, float z);
	Transform & Scale(const Vector3 &vector);
	Transform & SetScale(float x, float y, float z);
	Transform & SetScale(const Vector3 &vector);
	const Vector3 & GetScale() { return scale; }
	Transform & ResetScaling();

	Transform & Rotate(float x, float y ,float z);
	Transform & Rotate(const Vector3 &vector);
	Transform & SetRotate(float x, float y, float z);
	Transform & SetRotate(const Vector3 &vector);
	const Vector3 & GetRotate() { return rotate; }
	Transform & ResetRotation();

private:
	mutable Matrix4x4 matrix;
	mutable bool needCompute;
	Vector3 translate, scale, rotate;
};

}


#endif					//TRANSFORM_H_