#include "vector3.h"

#include "matrix4x4.h"

#include <stdio.h>
#include <math.h>

Vector3::Vector3() {
	Set(0.0f, 0.0f, 0.0f);
}

Vector3::Vector3(float _0, float _1, float _2) {
	Set(_0, _1, _2);
}

void Vector3::Normalize() {
	float distance =  Distance();
	data[0]/=distance;
	data[1]/=distance;
	data[2]/=distance;
}

float Vector3::Distance() {
	return sqrt(data[0]*data[0]+data[1]*data[1]+data[2]*data[2]);
}

Vector3 Vector3::Cross(const Vector3 &rhs) const {
	Vector3 result;
	result[0] = data[1]*rhs[2] - data[2]*rhs[1];
	result[1] = data[2]*rhs[0] - data[0]*rhs[2];
	result[2] = data[0]*rhs[1] - data[2]*rhs[0];
	return result;
}

float Vector3::Dot(const Vector3 &rhs) const {
	return data[0]*rhs[0] + data[1]*rhs[1] + data[2]*rhs[2];
}

Vector3 & Vector3::operator=(const Vector3 &rhs) {
	Set(rhs[0], rhs[1], rhs[2]);
	return *this;
}

Vector3 & Vector3::operator=(const Vector4 &rhs) {
	(*this) = rhs.ToVec3();
	return *this;
}

Vector3 & Vector3::operator+=(const Vector3 &rhs) {
	data[0] += rhs[0];
	data[1] += rhs[1];
	data[2] += rhs[2];
	
	return *this;
}

Vector3 & Vector3::operator-=(const Vector3 &rhs) {
	data[0] -= rhs[0];
	data[1] -= rhs[1];
	data[2] -= rhs[2];
	
	return *this;
}

Vector3 & Vector3::operator*=(float scale) {
	data[0] *= scale;
	data[1] *= scale;
	data[2] *= scale;
	
	return *this;
}

Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs) {
	Vector3 result;
	result[0] = lhs[0] + rhs[0];
	result[1] = lhs[1] + rhs[1];
	result[2] = lhs[2] + rhs[2];
	return result;
}

Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs) {
	Vector3 result;
	result[0] = lhs[0] - rhs[0];
	result[1] = lhs[1] - rhs[1];
	result[2] = lhs[2] - rhs[2];
	return result;
}

Vector3 operator*(const Vector3 &vec, float scale) {
	Vector3 result;
	result[0] = vec[0] * scale;
	result[1] = vec[1] * scale;
	result[2] = vec[2] * scale;
	return result;
}

Vector3 operator*(float scale, const Vector3 &vec) {
	return vec*scale;
}