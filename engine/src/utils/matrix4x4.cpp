#include "matrix4x4.h"

#include <math.h>
#include <assert.h>
#include <string.h>

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

Matrix4x4::Matrix4x4() {
	SetIdentity();
}

Matrix4x4::Matrix4x4(
			float _00, float _01, float _02, float _03, 
			float _10, float _11, float _12, float _13, 
			float _20, float _21, float _22, float _23, 
			float _30, float _31, float _32, float _33) {
	Set(	_00, _01, _02, _03, 
			_10, _11, _12, _13, 
			_20, _21, _22, _23, 
			_30, _31, _32, _33);	
}

namespace {

void SwapLine(float mat[4][8], int l1, int l2)
{
	if(l1 == l2)
		return;
	for(int i=0; i<8; i++)
	{
		float temp = mat[l1][i];
		mat[l1][i] = mat[l2][i];
		mat[l2][i] = temp;
	}
}

void ScaleLine(float mat[4][8], int line, float scale)
{
	for(int i=0; i<8; i++)
		mat[line][i] *= scale;
}

void AddLine(float mat[4][8], int des, int source ,float scale)
{
	for(int i=0; i<8; i++)
		mat[des][i]  += mat[source][i]*scale;
}

}

bool Matrix4x4::Invert() {
	//use Gauss Jordan elimination
	float augment[4][8];
	for(unsigned int i=0; i<4; i++) {
		for(unsigned int j=0; j<4; j++) {
			augment[i][j] = data[i][j];
			augment[i][j+4] = (i==j ? 1.0f : 0.0f);
		}
	}

	for(unsigned int i=0; i< 4; i++) {	
		//Scale
		int pivot=i;
		while(augment[pivot][i] == 0 && pivot <4) {
			pivot++;
		}
		if(pivot == 4)
			return false;
		SwapLine(augment, i, pivot);
		ScaleLine(augment, i, 1.0f/augment[i][i]);

		assert(abs(augment[i][i] - 1.0f) < 1e-6);

		//Eliminate
		for(unsigned int j=0; j<4; j++) {
			if(j == i) {
				continue;
			}
			AddLine(augment, j, i, -augment[j][i]);
		}
	}
	for(unsigned int i=0; i<4; i++) {
		for(unsigned int j=0; j<4; j++) {
			data[i][j] = augment[i][j+4];
		}
	}
	return true;
}

void Matrix4x4::Transpose() {
	for(unsigned int i=0; i<4; i++) {
		for(unsigned int j=i+1; j<4; j++) {
			float temp = data[i][j];
			data[i][j] = data[j][i];
			data[j][i] = temp;
		}
	}
}

Matrix4x4 & Matrix4x4::operator+=(const Matrix4x4 &rhs) {
	for(unsigned int i=0; i<4; i++) {
		for(unsigned int j=0; j<4; j++) {
			data[i][j] += rhs[i][j];
		}
	}
	return *this;
}

Matrix4x4 & Matrix4x4::operator-=(const Matrix4x4 &rhs) {
	for(unsigned int i=0; i<4; i++) {
		for(unsigned int j=0; j<4; j++) {
			data[i][j] -= rhs[i][j];
		}
	}
	return *this;
}

Matrix4x4 & Matrix4x4::operator*=(const Matrix4x4 &rhs) {
	for(unsigned int i=0; i<4; i++) {
		float temp[4] = {data[i][0], data[i][1], data[i][2], data[i][3]};
		for(unsigned int j=0; j<4; j++) {
			float value = 0.0f;
			for(unsigned int k=0; k<4; k++) {
				value += (temp[k]*rhs[k][j]);
			}
			data[i][j] = value;
		}
	}
	return *this;
}

Matrix4x4 & Matrix4x4::operator*=(float scale) {
	for(unsigned int i=0; i<4; i++) {
		for(unsigned int j=0; j<4; j++) {
			data[i][j] *= scale;
		}
	}
	return *this;
}

Matrix4x4 & Matrix4x4::SetProjection(float aspect, float fov, float np, float fp) {
	SetIdentity();
	float yscale = 1.0f/tan(fov/2);
	data[0][0] = yscale/aspect;
	data[1][1] = yscale;
	data[2][2] = fp/(np-fp);
	data[2][3] = np*fp/(np-fp);
	data[3][2] = -1.0f;
	data[3][3] = 0.0f;

	return *this;
}

Matrix4x4 & Matrix4x4::SetIdentity() {
	memset(data, 0, 64);
	data[0][0] = 1.0f;
	data[1][1] = 1.0f;
	data[2][2] = 1.0f;
	data[3][3] = 1.0f;

	return *this;
}

Matrix4x4 & Matrix4x4::SetRotationX(float angle) {
	float c=cos(angle), s=sin(angle);
	Set( 
		1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	c, 		-s,	0.0f,
		0.0f,	s,		c,		0.0f,
		0.0f,	0.0f,	0.0f, 1.0f);
	return *this;
}

Matrix4x4 & Matrix4x4::SetRotationY(float angle) {
	float c=cos(angle), s=sin(angle);
	Set(
		c,		0.0f,	s,		0.0f,
		0.0f,	1.0f, 0.0f,	0.0f,
		-s,	0.0f,	c,		0.0f,
		0.0f,	0.0f,	0.0f, 1.0f);
	return *this;
}

 Matrix4x4 & Matrix4x4::SetRotationZ(float angle) {
	float c=cos(angle), s=sin(angle);
	Set(
		c,		-s,	0.0f,	0.0f,
		s,		c, 		0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f, 1.0f);
	return *this;
}
	
Matrix4x4 operator+(const Matrix4x4 &lhs, const Matrix4x4 &rhs) {
	Matrix4x4 result;
	for(unsigned int i=0; i<4; i++) {
		for(unsigned int j=0; j<4; j++) {
			result[i][j] = lhs[i][j] + rhs[i][j];
		}
	}
	return result;
}

Matrix4x4 operator-(const Matrix4x4 &lhs, const Matrix4x4 &rhs) {
	Matrix4x4 result;
	for(unsigned int i=0; i<4; i++) {
		for(unsigned int j=0; j<4; j++) {
			result[i][j] = lhs[i][j] - rhs[i][j];
		}
	}
	return result;
}

Matrix4x4 operator*(const Matrix4x4 &lhs, const Matrix4x4 &rhs) {
	Matrix4x4 result;
	
	for(unsigned int i=0; i<4; i++) {
		for(unsigned int j=0; j<4; j++) {
			float value = 0.0f;
			for(unsigned int k=0; k<4; k++) {
				value += (lhs[i][k]*rhs[k][j]);
			}
			result[i][j] = value;
		}
	}
	return result;
}

Vector4 operator*(const Matrix4x4 &lhs, const Vector4 &rhs) {
	Vector4 result;
	
	for(unsigned int i=0; i<4; i++) {
		result[i] = 0.0f;
		for(unsigned int j=0; j<4; j++) {
			result[i] += (lhs[i][j]*rhs[j]);
		}
	}
	
	return result;
}

Vector4 operator*(const Vector4 &lhs, const Matrix4x4 &rhs) {
	Vector4 result;
	
	for(unsigned int i=0; i<4; i++) {
		result[i] = 0.0f;
		for(unsigned int j=0; j<4; j++) {
			result[i] += (lhs[j] * rhs[j][i]);
		}
	}
	
	return result;
}

Matrix4x4 operator*(const Matrix4x4 &matrix, float scale) {
	Matrix4x4 result;
	for(unsigned int i=0; i<4; i++) {
		for(unsigned int j=0; j<4; j++) {
			result[i][j] = matrix[i][j] * scale;
		}
	}
	return result;
}

Matrix4x4 operator*(float scale, const Matrix4x4 &matrix) {
	return matrix * scale;
}