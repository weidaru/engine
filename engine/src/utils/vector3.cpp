#include "vector4.h"

#include <stdio.h>
#include <math.h>

Vector3::Vector3() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

void Vector3::Normalize() {
	float distance =  Distance();
	x/=distance;
	y/=distance;
	z/=distance;
}

float Vector3::Distance() {
	return sqrt(x*x+y*y+z*z);
}

Vector3 Vector3::Cross(const Vector3 &rhs) const {
	Vector3 result;
	result.x = lhs.y*rhs.z - lhs.z*rhs.y;
	result.y = lhs.z*rhs.x - lhs.x*rhs.z;
	result.z = lhs.x*rhs.y - lhs.y*rhs.x;
	return result;
}

float Vector3::Dot(const Vector3 &rhs) const {
	return x*rhs.x + y*rhs.y + z*rhs.z;
}

Vector3 & Vector3::operator+=(const Vector3 &rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	
	return *this;
}

Vector3 & Vector3::operator-=(const Vector3 &rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
}

Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs) {
	Vector3 result;
	result.x = lhs.x + rhs.x;
	result.y = lhs.y + rhs.y;
	result.z = lhs.z + rhs.z;
	return result;
}

Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs) {
	Vector3 result;
	result.x = lhs.x - rhs.x;
	result.y = lhs.y - rhs.y;
	result.z = lhs.z - rhs.z;
	return result;
}