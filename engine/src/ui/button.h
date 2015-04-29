#ifndef WIDGET_H_
#define WIDGET_H_

#include "entity/entity.h"

#include <vector>
#include <functional>

namespace s2 {

class Button : public Entity {
public: 
	typedef  std::function<void (*)(Button *)> Callback;

public:
	Button();
	Button(float x, float y, float z, float width, float height);

	int AddClickCallback(Callback cb);
	void RemoveClickCallback(unsigned int index);

	virtual void OneFrame(float delta);

	float GetX() { return x; }
	Button & SetX(float new_value) { x = new_value; return *this; }
	float GetY() { return y; }
	Button & SetY(float new_value) { y = new_value; return *this; }
	float GetZ() { return z; }
	Button & SetZ(float new_value) { z = new_value; return *this; }
	float GetWidth() { return width; }
	Button & SetWidth(float new_value) { width = new_value; return *this; }
	float GetHeight() { return height; }
	Button & SetHeight(float new_value) { height = new_value; return *this; }

private:
	std::vector<Callback> click_callbacks;

	bool inClick;
	float x, y,z, width, height;
};

}


#endif				//WIDGET_H_