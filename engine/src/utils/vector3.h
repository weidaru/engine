#ifndef VECTOR4_H_
#define VECTOR4_H_

struct Vector3 {
	float x, y, z, w;
	
	Vector3();
	
	float Distance();
	void Normalize();
	
	Vector3 Cross(const Vector3 &rhs) const;
	float Dot(const Vector3 &rhs) const;
	Vector3 & operator+=(const Vector3 &rhs);
	Vector3 & operator-=(const Vector3 &rhs);
};

Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs);
Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs);


#endif			//VECTOR4_H_