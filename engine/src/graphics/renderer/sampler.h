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
		unsigned int 			max_anisotropy;
		ComparisonFunc 	compare_func;
		WrapMode 			u_wrap;
		WrapMode 			v_wrap;
		WrapMode 			w_wrap;
		float 					border_color[4];
		
		Option();
	};

public:
	virtual ~Sampler() {}
	virtual void Initialize(const Option &option) = 0;
	virtual const Sampler::Option & GetOption() const = 0;
};

}

#endif		//SAMPLER_H_