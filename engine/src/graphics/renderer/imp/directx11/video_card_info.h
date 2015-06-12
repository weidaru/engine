#ifndef VIDEO_CARD_INFO_H_
#define VIDEO_CARD_INFO_H_

#include "utils/s2string.h"
#include <stdint.h>

namespace s2 {
/**
 * Help class to choose the proper display mode according to resolution request.
 */
class VideoCardInfo {
public:
	struct AdapterInfo {
		char name[256];
		uint32_t refresh_numerator;
		uint32_t refresh_denominator;
		uint32_t memory_size;
	};	

public:
	static void GetProperAdapter(uint32_t screen_width, uint32_t screen_height,  AdapterInfo *info);
};
}

#endif			//VIDEO_CARD_INFO_H_	