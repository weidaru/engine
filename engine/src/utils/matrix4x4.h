#ifndef MATRIX4X4_H_
#define MATRIX4x4_H_

#include "vector3.h"

struct Vector4 {
	float x, y, z, w;
	
	Vector4();
	Vector4(const Vector3 &vec3, float _w);
	Vector3 ToVec3();
};

//[[TypeInfo]]//
struct Matrix4x4 {
	//[[CoreData]]//
	float data[4][4];

	Matrix4x4() {
		memset(data, 0, 64);
		data[0][0] = 1.0f;
		data[1][1] = 1.0f;
		data[2][2] = 1.0f;
		data[3][3] = 1.0f;
	}
	
	float * operator[](unsigned int index) {
		return data[index];
	}
	
	const float * operator[](unsigned int index) const {
		return data[index];
	}
};

static void MakeMatrix4x4Identity(Matrix *_m) {
	Matrix &m = *_m;
	memset(_m, 0, 64);
	m[0][0] = 1.0f;
	m[1][1] = 1.0f;
	m[2][2] = 1.0f;
	m[3][3] = 1.0f;
}

static void MakeIntrinsicRotationMatrix(Matrix *_m, float alpha, float beta, float gamma) {
	Matrix &m = *_m;
	m[0][0]=cos(theta); 		m[0][1]=0.0f;		m[0][2]=-sin(theta);		m[0][3]=0.0f;
	m[1][0]=0;			 		m[1][1]=1.0f;		m[1][2]=0.0f;				m[1][3]=0.0f;
	m[2][0]=sin(theta); 		m[2][1]=0.0f;		m[2][2]=cos(theta);		m[2][3]=0.0f;
	m[3][0]=0.0f;		 		m[3][1]=0.0f;		m[3][2]=0.0f;				m[3][3]=1.0f;
}

static void MakeExtrinsicRotationMatrix(Matrix *_m,  float alpha, float beta, float gamma) {
	Matrix &m = *_m;
	m[0][0]=cos(theta); 		m[0][1]=0.0f;		m[0][2]=-sin(theta);		m[0][3]=0.0f;
	m[1][0]=0;			 		m[1][1]=1.0f;		m[1][2]=0.0f;				m[1][3]=0.0f;
	m[2][0]=sin(theta); 		m[2][1]=0.0f;		m[2][2]=cos(theta);		m[2][3]=0.0f;
	m[3][0]=0.0f;		 		m[3][1]=0.0f;		m[3][2]=0.0f;				m[3][3]=1.0f;
}


#endif			//MATRIX4X4_H_