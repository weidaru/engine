#ifndef VECTOR4_H_
#define VECTOR4_H_

#include "vector3.h"

#include <stdint.h>

namespace s2 {

struct Vector4 {
	float data[4];
	
	Vector4();
	Vector4(const Vector3 &vec3, float _w);
	Vector4(float _0, float _1, float _2, float _3);
	Vector3 ToVec3() const;
	
	void Set(float _0, float _1, float _2, float _3) {
		data[0] = _0; data[1] = _1; data[2] = _2; data[3] = _3;
	}
	
	float & operator[](uint32_t index) { return data[index]; }
	const float & operator[](uint32_t index) const { return data[index]; }
};

}





#endif			//VECTOR4_H_