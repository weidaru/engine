#ifndef CAMERA_H_
#define CAMERA_H_

#include "utils/matrix4x4.h"
#include "utils/vector3.h"

namespace s2 {

/**
 * Assume column matrix and column vector
 */
class Camera {
public:
	Camera();
	
	void Reset();
	
	/**
	 * Turn around x axis relative to current frame. Positive is counter-clock-wise;
	 */
	void TurnAroundLocalX(float angle);
	
		/**
	 * Turn around y axis relative to current frame. Positive is counter-clock-wise.
	 */
	void TurnAroundLocalY(float angle);
	
	/**
	 * Turn around z axis relative to current frame. Positive is counter-clock-wise;
	 * This one is most likely unused.
	 */
	void TurnAroundLocalZ(float angle);
	
	const Matrix4x4 & GetViewMatrix();
	const Vector3 & GetPosition();
	const Vector3 & GetForwardVector();
	const Vector3 & GetUpVector();
	
	Camera & Move(const Vector3 &movement);
	
	Camera & SetPosition(const Vector3 &vec);
	/**
	 * This may adjust up vector too.
	 */
	Camera & SetForwardVector(const Vector3 &vec);
	/**
	 * Actual up vector may be adjusted to be vertical to forward vector.
	 */
	Camera & SetUpVector(const Vector3 &vec);
	
private:
	void CalculateVectors();
	
private:
	Vector3 position;
	Vector3 forward, forward_raw;
	Vector3 up, up_raw;
	
	float alpha, beta, gamma;

	Matrix4x4 matrix;
	bool calculate_vector;
	bool calculate_matrix;
};


}

#endif		//CAMERA_H_
