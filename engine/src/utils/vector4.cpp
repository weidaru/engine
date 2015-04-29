#include "vector4.h"

namespace s2 {


Vector4::Vector4() {
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
	data[3] = 0.0f;
}

Vector4::Vector4(const Vector3 &vec3, float _w) {
	data[0] = vec3[0];
	data[1] = vec3[1];
	data[2] = vec3[2];
	data[3] = _w;
}

Vector4::Vector4(float _0, float _1, float _2, float _3) {
	Set(_0, _1, _2, _3);
}

Vector3 Vector4::ToVec3() const {
	Vector3 result;
	if(data[3]==0.0f || data[3]==1.0f) {
		result[0] = data[0];
		result[1] = data[1];
		result[2] = data[2];
	} else {
		float w = data[3];
		result[0] = data[0]/w;
		result[1] = data[1]/w;
		result[2] = data[2]/w;
	}
	return result;
}


}
