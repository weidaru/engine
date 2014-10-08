#ifndef VIDEO_CARD_INFO_H_
#define VIDEO_CARD_INFO_H_

#include "utils/s2string.h"

namespace s2 {
/**
 * Help class to choose the proper display mode according to resolution request.
 */
class VideoCardInfo {
public:
	struct AdapterInfo {
		char name[256];
		unsigned int refresh_numerator;
		unsigned int refresh_denominator;
		unsigned int memory_size;
	};	

public:
	static void GetProperAdapter(unsigned int screen_width, unsigned int screen_height,  AdapterInfo *info);
};
}

#endif			//VIDEO_CARD_INFO_H_	