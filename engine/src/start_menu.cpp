#include "start_menu.h"

#include <glog/logging.h>

#include "engine.h"
#include "engine_program.h"
#include "entity/entity.h"
#include "entity/transform.h"
#include "graphics/text/text.h"
#include "graphics/text/text_system.h"
#include "input/input_system.h"

namespace s2 {

namespace {

void SetPosition(Entity *entity, float x, float y, float z) {
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = (float)setting.window_width, w_height = (float)setting.window_height;

	float s_x = x/w_width*2 - 1;
	float s_y = 1- y/w_height*2;
	float s_z = z;
	entity->GetTransform()->SetTranslate(s_x, s_y, s_z);
}

}

StartMenu::StartMenu(EngineProgramManager *manager)
		:	text_container(0), selected_index(0),
			selected_color(1.0f, 0.0f, 0.0f, 1.0f), unselected_color(1.0f, 1.0f, 1.0f, 1.0f),
			 check_interval(0.1f), check_timer(check_interval),
			 cur_state(SELECT){
	CHECK_NOTNULL(manager);

	text_container = new Entity();
	std::vector<EngineProgram *> raw_programs;
	manager->GetAll(&raw_programs);

	if(raw_programs.size() == 0) {
		return;
	}

	TextSystem *text_system = Engine::GetSingleton()->GetTextSystem();
	RendererSetting setting = Engine::GetSingleton()->GetRendererContext()->GetSetting();
	float w_width = (float)setting.window_width, w_height = (float)setting.window_height;

	std::vector<float> height_vector(raw_programs.size());
	std::vector<float> width_vector(raw_programs.size());
	for(uint32_t i=0; i<raw_programs.size(); i++) {
		Entity *text_entity = new Entity();
		EngineProgram *program = raw_programs[i];
		Text *text = new Text(text_entity);
		text_system->Register(text);
		text_container->AddChild(text_entity);
		text->SetContent(program->GetName());
		text->SetColor(unselected_color);
		text->Prepare();
		std::tuple<float, float> bound = text->GetBoundingSize();
		height_vector[i] = std::get<1>(bound);
		width_vector[i] = w_width/2.0f;
		programs.push_back({program, text});
	}

	float padding = 5.0f;
	uint32_t mid = height_vector.size()/2;
	float mid_height = height_vector[mid];
	float prev_height = mid_height;
	height_vector[mid] = w_height/2.0f;
	for(uint32_t i=mid+1; i<height_vector.size(); i++) {
		float cur_height = height_vector[i];
		height_vector[i] = height_vector[i-1] + (prev_height + cur_height)/2.0f + padding;
		prev_height = cur_height;
	}

	prev_height = mid_height;
	for(int i=mid-1; i>=0; i--) {
		float cur_height = height_vector[i];
		height_vector[i] = height_vector[i+1] - (prev_height + cur_height)/2.0f - padding;
		prev_height = cur_height;
	}

	for(uint32_t i=0; i<height_vector.size(); i++) {
		EngineProgram *program = programs[i].program;
		Text *text = programs[i].text;
		Entity *e = text->GetEntity();
		SetPosition(e, width_vector[i], height_vector[i], 0.0f);
	}
	SetSelectedIndex(mid);

}

void StartMenu::OnEnter() {
	text_container->SetEnabled(true);
	Engine::GetSingleton()->GetInputSystem()->SetMouseVisible(false);
}

void StartMenu::OnLeave() {
	text_container->SetEnabled(false);
}

StartMenu::~StartMenu() {
	delete text_container;
}

void StartMenu::SetSelectedIndex(uint32_t index) {
	if(index >= programs.size()) {
		return;
	} else {
		programs[selected_index].text->SetColor(unselected_color);
		selected_index = index;
		programs[selected_index].text->SetColor(selected_color);
	}
}

void StartMenu::SetSelectedProgram(const s2string &name) {
	for (uint32_t i = 0; i < programs.size(); i++) {
		if (name == programs[i].program->GetName()) {
			SetSelectedIndex(i);
			break;
		}
	}
}

void StartMenu::OneFrame(float delta) {
	InputSystem *input_system = Engine::GetSingleton()->GetInputSystem();
	
	if(cur_state == SELECT) {
		if(input_system->IsKeyDown(InputSystem::K_UP)) {
			if(selected_index != 0 ) {
				SetSelectedIndex(selected_index-1);
				check_timer = check_interval;
				cur_state = SELECT_INTERVAL;
			}
		} else if(input_system->IsKeyDown(InputSystem::K_DOWN)) {
			SetSelectedIndex(selected_index+1);
			check_timer = check_interval;
			cur_state = SELECT_INTERVAL;
		}
	} else if(cur_state == SELECT_INTERVAL) {
		check_timer -= delta;
		if(check_timer < 0) {
			cur_state = SELECT;
		}
	}

}

}

	