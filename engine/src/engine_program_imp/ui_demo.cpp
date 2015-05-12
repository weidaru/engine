#include <glog/logging.h>

#include "engine_program.h"
#include "engine.h"

#include "graphics/renderer/all.h"
#include "graphics/sprite_system.h"
#include "graphics/sprite.h"

#include "input/input_system.h"

#include "entity/entity.h"
#include "entity/entity_system.h"

#include "ui/button.h"

namespace s2 {

class UIDemo : public EngineProgram {
public:
	UIDemo() {

	}

	virtual ~UIDemo() {
	}

	virtual bool Initialize() {
		button = new Button();
		Sprite *sprite = button->GetSprite();
		sprite->SetAbsolutePosition(100.0f, 100.0f, 0.0f);
		sprite->SetAbsoluteWidth(100.0f);
		sprite->SetHeight(0.2f);
		sprite->SetBackgroundColor(Vector4(0.5f, 0.0f, 0.0f, 1.0f));

		button->AddClickCallback([] (Button *self) {
			Sprite *sprite = self->GetSprite();
			if(sprite->GetBackgroundColor()[0] > 0.0f) {
				sprite->SetBackgroundColor(Vector4(0.0f, 0.5f, 0.0f, 1.0f));	
			} else {
				self->GetSprite()->SetBackgroundColor(Vector4(0.5f, 0.0f, 0.0f, 1.0f));
			}
		});
		button->SetEnabled(false);

		return true;
	}
	
	virtual s2string GetName() const {
		return "UIDemo";
	}

	virtual void OneFrame(float delta) {
		button->SetEnabled(true);
		button->OneFrame(delta);
	}
	
private:
	Button *button;
};

AddBeforeMain(UIDemo)

}

