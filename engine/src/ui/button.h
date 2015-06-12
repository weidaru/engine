#ifndef WIDGET_H_
#define WIDGET_H_

#include "entity/entity.h"

#include <vector>
#include <functional>
#include <stdint.h>

namespace s2 {

class Sprite;
class Text;

class Button : public Entity {
public: 
	typedef  std::function<void (Button *)> Callback;

public:
	Button(EntitySystem *system);

	virtual ~Button() override;

	int AddClickCallback(Callback cb);
	void RemoveClickCallback(uint32_t index);

	virtual Button & SetEnabled(bool new_value) override;

	Button & SetFontSize(uint32_t size);
	uint32_t GetFontSize();

	Button & SetText(const s2string &content);
	const s2string & GetText();

	Button & SetTextColor(const Vector4 &color);
	const Vector4 & GetTextColor();

	Button & SetBackgroundColor(const Vector4 &color);
	const Vector4 & GetBackgroundColor();

	const Vector3 & GetPosition();
	Button & SetPosition(float x, float y, float z);
	Button & SetPosition(const Vector3 &vec);

	Vector3 GetAbsolutePosition();
	Button & SetAbsolutePosition(float x, float y, float z);
	Button & SetAbsolutePosition(const Vector3 &vec);
	
	float GetAbsoluteWidth();
	Button & SetAbsoluteWidth(float new_value);
	float GetWidth();
	Button & SetWidth(float new_value);

	float GetAbsoluteHeight();
	Button & SetAbsoluteHeight(float new_value);
	float GetHeight();
	Button & SetHeight(float new_value);

	float GetDepth();
	Button & SetDepth(float new_value);

protected:
	virtual void OneFrame(float delta);

private:
	std::vector<Callback> click_callbacks;

	bool inClick;
	Sprite *sprite;
	Text *text;
};

}


#endif				//WIDGET_H_