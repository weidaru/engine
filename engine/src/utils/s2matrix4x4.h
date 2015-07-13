#ifndef S2MATRIX4x4_H_
#define S2MATRIX4x4_H_

#include "s2vector4.h"
#include "s2vector3.h"

#include <stdint.h>

/**
 * Row major memory layout.
 */
//[[TypeInfo]]//
struct S2Matrix4x4 {
	//[[CoreData]]//
	float data[4][4];

	S2Matrix4x4();
	S2Matrix4x4(
			float _00, float _01, float _02, float _03, 
			float _10, float _11, float _12, float _13, 
			float _20, float _21, float _22, float _23, 
			float _30, float _31, float _32, float _33);

	
	bool Invert();
	void Transpose();
	
	float * operator[](uint32_t index) { return data[index]; }
	const float * operator[](uint32_t index) const { return data[index]; }
	
	void Set(
			float _00, float _01, float _02, float _03, 
			float _10, float _11, float _12, float _13, 
			float _20, float _21, float _22, float _23, 
			float _30, float _31, float _32, float _33)  {
		data[0][0] = (_00); data[0][1] = (_01); data[0][2] = (_02); data[0][3] = (_03); 
		data[1][0] = (_10); data[1][1] = (_11); data[1][2] = (_12); data[1][3] = (_13); 
		data[2][0] = (_20); data[2][1] = (_21); data[2][2] = (_22); data[2][3] = (_23); 
		data[3][0] = (_30); data[3][1] = (_31); data[3][2] = (_32); data[3][3] = (_33);
	}
	
	S2Matrix4x4 & SetIdentity();
	S2Matrix4x4 & SetRotationX(float angle);
	S2Matrix4x4 & SetRotationY(float angle);
	S2Matrix4x4 & SetRotationZ(float angle);
	//Projection is LH.
	S2Matrix4x4 & SetProjection(float aspect, float fov, float np, float fp);
	S2Matrix4x4 & SetTranslate(const S2Vector3& vec);
	S2Matrix4x4 & SetTranslate(float x, float y, float z);
	S2Matrix4x4 & SetScale(const S2Vector3& vec);
	S2Matrix4x4 & SetScale(float x, float y, float z);
	
	S2Matrix4x4 & operator+=(const S2Matrix4x4 &rhs);
	S2Matrix4x4 & operator-=(const S2Matrix4x4 &rhs);
	S2Matrix4x4 & operator*=(const S2Matrix4x4 &rhs);
	S2Matrix4x4 & operator*=(float scale);
};

S2Matrix4x4 operator+(const S2Matrix4x4 &lhs, const S2Matrix4x4 &rhs);
S2Matrix4x4 operator-(const S2Matrix4x4 &lhs, const S2Matrix4x4 &rhs);
S2Matrix4x4 operator*(const S2Matrix4x4 &lhs, const S2Matrix4x4 &rhs);
S2Matrix4x4 operator*(const S2Matrix4x4 &matrix, float scale);
S2Matrix4x4 operator*( float scale, const S2Matrix4x4 &matrix);

S2Vector4 operator*(const S2Matrix4x4 &lhs, const S2Vector4 &rhs);
S2Vector4 operator*(const S2Vector4 &lhs, const S2Matrix4x4 &rhs);



#endif			//S2MATRIX4X4_H_