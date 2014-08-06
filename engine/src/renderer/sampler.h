#ifndef SAMPLER_H_
#define SAMPLER_H_

#include "resource.h"

namespace s2 {
class Sampler : public Resource {
public:
	enum Filter {
		NEAREST,
		LINEAR,
		/**
		 * Care! Set this to one of Mag, Min and Mip filter will set all the others. 
		 * At least that is all the current implementation do.
		 */
		ANISOTROPIC			
	};
	
	enum WrapMode {
		REPEAT,
		CLAMP,
		MIRROR,
		Border
	};

	struct Option {
		Filter 			MagFilter;
		Filter 			MinFilter;
		Filter 			MipFilter;	//Mip filter is only applied when texture minification is desired.
		WrapMode 		UWrap;
		WrapMode 		VWrap;
		WrapMode 		WWrap;
		unsigned int 	MaxAnisotropy;
		float 			BorderColor[4];
	};

public:
	virtual ~Sampler() {}
	virtual void Initialize(const Option &option) = 0;
	virtual void GetOption(Option *option) = 0;
};

}

#endif		//SAMPLER_H_