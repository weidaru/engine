#ifndef S2VECTOR4_H_
#define S2VECTOR4_H_

#include "s2vector3.h"

#include <stdint.h>

//[[TypeInfo]]//
struct S2Vector4 {
	float data[4];
	
	S2Vector4();
	S2Vector4(const S2Vector3 &vec3, float _w);
	S2Vector4(float _0, float _1, float _2, float _3);
	S2Vector3 ToVec3() const;
	
	void Set(float _0, float _1, float _2, float _3) {
		data[0] = _0; data[1] = _1; data[2] = _2; data[3] = _3;
	}
	
	float & operator[](uint32_t index) { return data[index]; }
	const float & operator[](uint32_t index) const { return data[index]; }
};

#endif			//S2VECTOR4_H_