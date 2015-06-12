#ifndef PIXEL_MAP_H_
#define PIXEL_MAP_H_

#include "utils/s2string.h"
#include "graphics/renderer/texture_enum.h"
#include "graphics/renderer/texture2d.h"

struct FIBITMAP;

namespace s2 {

class PixelMap {
public:
	enum Format {
		R8G8B8A8
	};

public:
	PixelMap();
	~PixelMap();
	
	bool Initialize(const s2string &_path, Format _format);
	const s2string &GetLastError() { return error;}
	
	void * GetRawMemory();
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	const s2string & GetPath() const;
	PixelMap::Format GetFormat() const;
	
	void PopulateTexture2DOption(Texture2D::Option *option);
	
private:
	void Check() const;
	void Clear();
	
private:
	FIBITMAP *bitmap;
	s2string path;
	Format format;
	s2string error;
};


}


#endif		//PIXEL_MAP_H_
