#ifndef S2VECTOR3_H_
#define S2VECTOR3_H_

#include <stdint.h>

struct S2Vector4;

//[[TypeInfo]]//
struct S2Vector3 {
	float data[3];
	
	S2Vector3();
	S2Vector3(float _0, float _1, float _2);
	
	float Distance();
	void Normalize();
	
	float & operator[](uint32_t index) { return data[index]; }
	const float & operator[](uint32_t index) const { return data[index]; }
	
	void Set(float _0, float _1, float _2) {
		data[0] = _0; data[1] = _1; data[2] = _2;
	}
	
	S2Vector3 Cross(const S2Vector3 &rhs) const;
	float Dot(const S2Vector3 &rhs) const;
	
	S2Vector3 & operator=(const S2Vector3 &rhs);
	S2Vector3 & operator=(const S2Vector4 &rhs);
	
	S2Vector3 & operator+=(const S2Vector3 &rhs);
	S2Vector3 & operator-=(const S2Vector3 &rhs);
	
	S2Vector3 & operator*=(float scale);
};

S2Vector3 operator+(const S2Vector3 &lhs, const S2Vector3 &rhs);
S2Vector3 operator-(const S2Vector3 &lhs, const S2Vector3 &rhs);
S2Vector3 operator*(const S2Vector3 &vec, float scale);
S2Vector3 operator*(float scale, const S2Vector3 &vec);




#endif			//S2VECTOR3_H_