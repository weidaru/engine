#ifndef MATRIX4X4_H_
#define MATRIX4x4_H_

#include "vector3.h"

struct Vector4 {
	float data[4];
	
	Vector4();
	Vector4(const Vector3 &vec3, float _w);
	Vector4(float _0, float _1, float _2, float _3);
	Vector3 ToVec3() const;
	
	void Set(float _0, float _1, float _2, float _3) {
		data[0] = _0; data[1] = _1; data[2] = _2; data[3] = _3;
	}
	
	float & operator[](unsigned int index) { return data[index]; }
	const float & operator[](unsigned int index) const { return data[index]; }
};


/**
 * Row major memory layout.
 */
//[[TypeInfo]]//
struct Matrix4x4 {
	//[[CoreData]]//
	float data[4][4];

	Matrix4x4();
	Matrix4x4(
			float _00, float _01, float _02, float _03, 
			float _10, float _11, float _12, float _13, 
			float _20, float _21, float _22, float _23, 
			float _30, float _31, float _32, float _33);
	
	bool Invert();
	void Transpose();
	
	float * operator[](unsigned int index) { return data[index]; }
	const float * operator[](unsigned int index) const { return data[index]; }
	
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
	
	Matrix4x4 & SetIdentity();
	Matrix4x4 & SetRotationX(float angle);
	Matrix4x4 & SetRotationY(float angle);
	Matrix4x4 & SetRotationZ(float angle);
	Matrix4x4 & SetProjection(float aspect, float fov, float np, float fp);
	
	Matrix4x4 & operator+=(const Matrix4x4 &rhs);
	Matrix4x4 & operator-=(const Matrix4x4 &rhs);
	Matrix4x4 & operator*=(const Matrix4x4 &rhs);
	Matrix4x4 & operator*=(float scale);
};

Matrix4x4 operator+(const Matrix4x4 &lhs, const Matrix4x4 &rhs);
Matrix4x4 operator-(const Matrix4x4 &lhs, const Matrix4x4 &rhs);
Matrix4x4 operator*(const Matrix4x4 &lhs, const Matrix4x4 &rhs);
Matrix4x4 operator*(const Matrix4x4 &matrix, float scale);
Matrix4x4 operator*( float scale, const Matrix4x4 &matrix);

Vector4 operator*(const Matrix4x4 &lhs, const Vector4 &rhs);
Vector4 operator*(const Vector4 &lhs, const Matrix4x4 &rhs);



#endif			//MATRIX4X4_H_