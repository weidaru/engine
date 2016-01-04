#include <glog/logging.h>

#include "engine_program.h"
#include "engine.h"

#include "graphics/renderer/all.h"
#include "graphics/sprite/sprite_system.h"
#include "graphics/sprite/sprite.h"

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
		button = new Button(&entity_system);
		button->SetFontSize(64).
			SetTextColor(S2Vector4(1.0f, 1.0f, 1.0f, 1.0f)).
			SetText("Click").
			SetAbsoluteWidth(100.0f).
			SetAbsoluteHeight(80.0f).
			SetBackgroundColor(S2Vector4(0.5f, 0.0f, 0.0f, 1.0f));

		button->AddClickCallback([] (Button *self) {
			if(self->GetBackgroundColor()[0] > 0.0f) {
				self->SetBackgroundColor(S2Vector4(0.0f, 0.5f, 0.0f, 1.0f));	
			} else {
				self->SetBackgroundColor(S2Vector4(0.5f, 0.0f, 0.0f, 1.0f));
			}
		});

		return true;
	}
	
	virtual s2string GetName() const {
		return "UIDemo";
	}

	virtual void OneFrame(float delta) {
	}

	virtual void OnEnter() override {
		Engine::GetSingleton()->GetInputSystem()->SetMouseVisible(true);
	}
	
private:
	Button *button;
};

AddBeforeMain(UIDemo)

}

