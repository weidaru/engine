#ifndef WIDGET_H_
#define WIDGET_H_

#include "entity/entity.h"

#include <vector>
#include <functional>

namespace s2 {

class Sprite;

class Button : public Entity {
public: 
	typedef  std::function<void (Button *)> Callback;

public:
	Button();

	virtual ~Button() override;

	int AddClickCallback(Callback cb);
	void RemoveClickCallback(unsigned int index);

	Sprite * GetSprite() { return sprite; }

	virtual void OneFrame(float delta);

private:
	std::vector<Callback> click_callbacks;

	bool inClick;
	Sprite *sprite;

};

}


#endif				//WIDGET_H_