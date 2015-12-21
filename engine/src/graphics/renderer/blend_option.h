#ifndef BLEND_H_
#define BLEND_H_

#include <stdint.h>
#include <vector>
#include <math.h>

namespace s2 {

struct BlendOption {
	enum BlendFactor {
		ZERO,
		ONE,
		SRC_COLOR,
		INV_SRC_COLOR,
		SRC_ALPHA,
		INV_SRC_ALPHA,
		DEST_COLOR,
		INV_DEST_COLOR,
		DEST_ALPHA,
		INV_DEST_ALPHA,
		BLEND_FACTOR,
		INV_BLEND_FACTOR,
	};
	
	enum BlendOp {
		ADD,
		SUBTRACT,
		REV_SUBTRACT,
		MIN,
		MAX
	};
	
	struct RenderTargetBlendOption {
		BlendFactor src_factor;
		BlendFactor dest_factor;
		BlendOp op;
		BlendFactor src_alpha_factor;
		BlendFactor dest_alpha_factor;
		BlendOp alpha_op;
		uint8_t write_mask;
		
		//Default behaviour is replacing.
		RenderTargetBlendOption() {
			src_factor = ONE;	
			dest_factor = ZERO;
			op = ADD;
			src_alpha_factor = ONE;
			dest_alpha_factor = ZERO;
			alpha_op = ADD;
			write_mask = 0x0F;			//RGBA
		}

	};
	
	bool enable;
	//bool enable_alpha_to_coverage;				//This is multisampling related, disable for now.
	//I suppose it refers to samples in standard patterns. http://msdn.microsoft.com/en-us/library/windows/desktop/ff476218(v=vs.85).aspx
	uint32_t sample_mask;							
	std::vector<RenderTargetBlendOption> rt_options;

	BlendOption() {
		enable = false;
		sample_mask = 0xFFFFFFFF;
		rt_options.push_back(RenderTargetBlendOption());
	}
};


}


#endif		//BLEND_H_
