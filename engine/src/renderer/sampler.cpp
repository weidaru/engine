#include "sampler.h"

namespace s2 {
	
Sampler::Option::Option() {
	min_mag_filter = LINEAR;
	mip_filter = LINEAR;
	max_anisotropy = 0;
	compare_func = NEVER;
	u_wrap = CLAMP;
	v_wrap = CLAMP;
	w_wrap = CLAMP;
	border_color[0] = 0.0f;
	border_color[1] = 0.0f;
	border_color[2] = 0.0f;
	border_color[3] = 0.0f;
}

}