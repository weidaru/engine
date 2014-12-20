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

	void Update(int delta_x, int delta_y);
	void PostFrame(float delta);
	
	int GetMouseX() const { return mouse_x; }
	int GetMouseXDelta() const {  return mouse_x_delta; }
	int GetMouseY() const { return mouse_y; }
	int GetMouseYDelta() const { 
		int a=0;
		a++;
		return mouse_y_delta; 
	}
	
	bool IsMouseButtonDown(MouseButton button) const;
	
	bool IsKeyDown(Key key) const;
	
private:
	void *hwnd;
	std::vector<unsigned char> current;
	int mouse_x, mouse_y;
	int mouse_x_delta, mouse_y_delta;
};


}

#endif			//INPUT_SYSTEM_H_