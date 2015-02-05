#ifndef RASTERIZATION_OPTION_H_
#define RASTERIZATION_OPTION_H_

#include <vector>

namespace s2 {

//TODO: Add depth bias later.
struct RasterizationOption {
	struct Rectangle {
		float top_left_x;
		float top_left_y;
		float width;
		float height;
		
		Rectangle(float _top_left_x, float _top_left_y, float _width, float _height) {
			top_left_x = _top_left_x;
			top_left_y = _top_left_y;
			width = _width;
			height = _height;
		}
	};
	
	enum FillMode {
		WIREFRAME,
		SOLID
	};
	
	enum CullMode {
		NONE,
		FRONT,
		BACK
	};
	
	enum FaceOrientation {
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};

	std::vector<Rectangle> viewports;
	bool enable_scissor;
	std::vector<Rectangle> scissors;
	FillMode fill_mode;
	CullMode cull_mode;
	FaceOrientation face_orient;
	
	RasterizationOption() {
		enable_scissor = false;
		fill_mode = SOLID;
		cull_mode = BACK;
		face_orient = COUNTER_CLOCKWISE;
	}
};


}


#endif		//RASTERIZATION_OPTION_H_
