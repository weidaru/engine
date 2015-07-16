#ifndef TRANSFORM_H_
#define TRANSFORM_H_

#include "component.h"
#include "utils/s2matrix4x4.h"
#include "utils/s2vector3.h"

namespace s2 {

class Transform : public Component {
public:
	Transform(Entity *entity);

	const S2Matrix4x4 & GetPrefixMatrix() const;
	Transform & SetPrefixMatrix(const S2Matrix4x4 &m);

	const S2Matrix4x4 & GetSuffixMatrix() const;
	Transform & SetSuffixMatrix(const S2Matrix4x4 &m);

	const S2Matrix4x4 & GetMatrix() const;
	S2Matrix4x4 CalculateTranslateMatrix() const;
	S2Matrix4x4 CalculateScaleMatrix() const;
	S2Matrix4x4 CalcualteRotateMatrix() const;

	Transform & Translate(float x, float y, float z);
	Transform & Translate(const S2Vector3 &vector);
	Transform & SetTranslate(float x, float y, float z);
	Transform & SetTranslate(const S2Vector3 &vectro);
	const S2Vector3 & GetTranslate() const { return translate; }
	Transform & ResetTranslate();
	

	Transform & Scale(float x, float y, float z);
	Transform & Scale(const S2Vector3 &vector);
	Transform & SetScale(float x, float y, float z);
	Transform & SetScale(const S2Vector3 &vector);
	const S2Vector3 & GetScale() const { return scale; }
	Transform & ResetScaling();
	

	Transform & Rotate(float x, float y ,float z);
	Transform & Rotate(const S2Vector3 &vector);
	Transform & SetRotate(float x, float y, float z);
	Transform & SetRotate(const S2Vector3 &vector);
	const S2Vector3 & GetRotate() const { return rotate; }
	Transform & ResetRotation();

private:
	S2Matrix4x4 prefix_matrix, suffix_matrix;

	mutable S2Matrix4x4 matrix;
	mutable bool need_compute;
	S2Vector3 translate, scale, rotate;
};

}


#endif					//TRANSFORM_H_