#ifndef VECTOR3_H_
#define VECTOR3_H_

#include <stdint.h>

struct Vector4;

//[[TypeInfo]]//
struct Vector3 {
	float data[3];
	
	Vector3();
	Vector3(float _0, float _1, float _2);
	
	float Distance();
	void Normalize();
	
	float & operator[](uint32_t index) { return data[index]; }
	const float & operator[](uint32_t index) const { return data[index]; }
	
	void Set(float _0, float _1, float _2) {
		data[0] = _0; data[1] = _1; data[2] = _2;
	}
	
	Vector3 Cross(const Vector3 &rhs) const;
	float Dot(const Vector3 &rhs) const;
	
	Vector3 & operator=(const Vector3 &rhs);
	Vector3 & operator=(const Vector4 &rhs);
	
	Vector3 & operator+=(const Vector3 &rhs);
	Vector3 & operator-=(const Vector3 &rhs);
	
	Vector3 & operator*=(float scale);
};

Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs);
Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs);
Vector3 operator*(const Vector3 &vec, float scale);
Vector3 operator*(float scale, const Vector3 &vec);




#endif			//VECTOR4_H_