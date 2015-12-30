#ifndef DEPTH_STENCIL_OPTION_H_
#define DEPTH_STENCIL_OPTION_H_

#include <stdint.h>

namespace s2 {

struct DepthStencilOption {
	enum ComparisonFunc {
		NEVER,
		ALWAYS,
		EQUAL,
		NOT_EQUAL,
		LESS,
		LESS_EQUAL,
		GREATER,
		GREATER_EQUAL
	};
	
	enum StencilOp {
		KEEP,
		ZERO,
		REPLACE,
		INC_SAT,
		DEC_SAT,
		INVERT,
		INC,
		DEC
	};

	bool enable_depth;
	bool enable_depth_write;
	ComparisonFunc depth_compare;
	
	bool enable_stencil;
	uint8_t stencil_read_mask;
	uint8_t stencil_write_mask;
	ComparisonFunc front_compare;
	StencilOp front_fail;
	StencilOp front_pass;
	StencilOp front_stencil_pass_depth_fail;
	ComparisonFunc back_compare;
	StencilOp back_fail;
	StencilOp back_pass;
	StencilOp back_stencil_pass_depth_fail;
	
	DepthStencilOption() {
		enable_depth = true;
		enable_depth_write = true;
		depth_compare = LESS;
		
		enable_stencil = false;
		stencil_read_mask = 0xFF;
		stencil_write_mask = 0xFF;
		front_compare = NEVER;
		front_fail = KEEP;
		front_pass = KEEP;
		front_stencil_pass_depth_fail = KEEP;
		back_compare = NEVER;
		back_fail = KEEP;
		back_pass = KEEP;
		back_stencil_pass_depth_fail = KEEP;
	}
};


}


#endif		//DEPTH_STENCIL_OPTION_H_
