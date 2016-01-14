#ifndef SAMPLER_H_
#define SAMPLER_H_

#include "resource.h"

#include "utils/s2string.h"

namespace s2 {
class Sampler : public Resource {
public:
	enum Filter {
		NEAREST,
		LINEAR	
	};
	
	enum WrapMode {
		REPEAT,
		CLAMP,
		MIRROR,
		BORDER
	};
	
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

	struct Option {
		Filter 					min_mag_filter;
		Filter 					mip_filter;	//Mip filter is only applied when texture minification is desired.
		uint32_t 			max_anisotropy;
		ComparisonFunc 	compare_func;
		WrapMode 			u_wrap;
		WrapMode 			v_wrap;
		WrapMode 			w_wrap;
		float 					border_color[4];
		
		Option() {
			min_mag_filter = LINEAR;
			mip_filter = LINEAR;
			max_anisotropy = 0;
			compare_func = NEVER;
			u_wrap = CLAMP;
			v_wrap = CLAMP;
			w_wrap = CLAMP;
			border_color[0] = 1.0f;
			border_color[1] = 1.0f;
			border_color[2] = 1.0f;
			border_color[3] = 1.0f;
		}
	};

public:
	virtual ~Sampler() {}
	virtual void Initialize(const Option &option) = 0;
	virtual const Sampler::Option & GetOption() const = 0;
};

}

#endif		//SAMPLER_H_