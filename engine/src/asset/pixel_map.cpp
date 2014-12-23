#include "pixel_map.h"

#include <glog/logging.h>

#include "FreeImage.h"

namespace s2 {


PixelMap::PixelMap()
	: bitmap(0) {
	
}

PixelMap::~PixelMap() {
	Clear();
}

bool PixelMap::Initialize(const s2string &_path, Format _format) {
	Clear();
	path = _path;
	format = _format;
	
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(path.c_str(), 0);
	if(fif == FIF_UNKNOWN) {
		fif = FreeImage_GetFIFFromFilename(path.c_str());
	}
	if(fif == FIF_UNKNOWN) {
		S2StringFormat(&error, "Unknown format for %s", path);
		return false;
	}
	
	if(FreeImage_FIFSupportsReading(fif)) {
		bitmap = FreeImage_Load(fif, path.c_str());
	} else {
		S2StringFormat(&error, "File format does no support read.");
		return false;
	}
	if(!bitmap) {
		S2StringFormat(&error, "Cannot load file %s", path);
		return false;
	}

	
	switch(format) { 
	case R8G8B8A8:
		{
			FREE_IMAGE_TYPE type = FreeImage_GetImageType(bitmap);
			unsigned int bpp = FreeImage_GetBPP(bitmap);
			if(type!=FIT_BITMAP || bpp!=32) {
				FIBITMAP *bitmap_32 = FreeImage_ConvertTo32Bits(bitmap);
				CHECK(bitmap_32)<<"Cannot convert to 32bit bitmap";
				FreeImage_Unload(bitmap);
				bitmap = bitmap_32;
			}
		}
		break;
	default:
		CHECK(false)<<"Unsupported format";
		break;
	}
}

void PixelMap::Clear() {
	if(bitmap) {
		FreeImage_Unload(bitmap);
	}
	
	bitmap = 0;
	
}

void PixelMap::PopulateTexture2DOption(Texture2D::Option *option) {
	option->data = GetRawMemory();
	option->width = GetWidth();
	option->height = GetHeight();
	switch(format) {
	case R8G8B8A8:
		option->format = TextureEnum::R8G8B8A8_UNORM;
		break;
	default :
		CHECK(false)<<"Unsupported format "<<format;
		break;
	}
}

unsigned int PixelMap::GetWidth() const {
	Check();
	return FreeImage_GetWidth(bitmap);
}

unsigned int PixelMap::GetHeight() const {
	Check();
	return FreeImage_GetHeight(bitmap);
}
	
void * PixelMap::GetRawMemory() {
	Check();
	return (void *)FreeImage_GetBits(bitmap);
}

const s2string & PixelMap::GetPath() const {
	Check();
	return path;
}

PixelMap::Format PixelMap::GetFormat() const {
	Check();
	return format;
}

void PixelMap::Check() const {
	CHECK_NOTNULL(bitmap);
}



}

