#ifndef PIXEL_MAP_H_
#define PIXEL_MAP_H_

#include "utils/s2string.h"
#include "renderer/texture_enum.h"

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
	
	const void * GetRawMemory() const;
	const s2string & GetPath() const;
	PixelMap::Format GetFormat() const;
	
private:
	void Check();
	void Clear();
	
private:
	FIBITMAP *bitmap;
	s2string path;
	Format format;
	s2string error;
};


}


#endif		//PIXEL_MAP_H_
