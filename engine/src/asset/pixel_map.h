#ifndef PIXEL_MAP_H_
#define PIXEL_MAP_H_

#include "utils/s2string.h"
#include "graphics/renderer/texture2d.h"
#include "image_common.h"

struct FIBITMAP;

namespace s2 {

class PixelMap {
public:
	PixelMap();
	~PixelMap();
	
	bool Initialize(const s2string &_path, ImagePixelFormat _format);
	const s2string &GetLastError() { return error;}
	
	void * GetRawMemory();
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	const s2string & GetPath() const;
	ImagePixelFormat GetFormat() const;
	
	void PopulateTexture2DOption(Texture2D::Option *option);
	
private:
	void Check() const;
	
private:
	FIBITMAP *bitmap;
	s2string path;
	ImagePixelFormat format;
	s2string error;
};


}


#endif		//PIXEL_MAP_H_
