#ifndef INPUT_SYSTEM_H_
#define INPUT_SYSTEM_H_

#include <vector>

struct tagMSG;
typedef struct tagMSG MSG;

namespace s2 {

class InputSystem {
public:
	enum Key {
		K_0, K_1, K_2, K_3, K_4, 
		K_5, K_6, K_7, K_8, K_9,
		K_A, K_B, K_C, K_D, K_E,
		K_F, K_G, K_H, K_I, K_J, 
		K_K, K_L, K_M, K_N, K_O,
		K_P, K_Q, K_R, K_S, K_T,
		K_U, K_V, K_W, K_X, K_Y,
		K_Z,
		
		K_SPACE
	};
	
	enum MouseButton {
		M_LEFT,
		M_MID,
		M_RIGHT,
	};

public:
	InputSystem(void *hwnd);
	virtual ~InputSystem() {}

	void OneFrame(float delta);
	
	int GetMouseX() const { return mouse_x; }

	/**
	 * Result between -1.0f and 1.0f
	 */
	float GetMouseXMove() const;
	int GetMouseY() const { return mouse_y; }
	/**
	 * Result between -1.0f and 1.0f
	 */
	float GetMouseYMove() const;

	void SetMousePosition(int x, int y);
	void SetMousePositionPercent(float x, float y);
	
	bool IsMouseButtonDown(MouseButton button) const;
	
	bool IsKeyDown(Key key) const;
	
private:
	void *hwnd;
	std::vector<unsigned char> current;
	int mouse_x, mouse_y;
	float mouse_x_delta, mouse_y_delta;

	float timer;
};


}

#endif			//INPUT_SYSTEM_H_