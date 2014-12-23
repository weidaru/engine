#include "pixel_map.h"

#include <glob/logging.h>

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
	fif = FreeImage_GetFileType(filename, 0);
	if(fif == FIF_UNKNOWN) {
		fif = FreeImage_GetFIFFromFilename(filename);
	}
	if(fif == FIF_UNKNOWN) {
		S2StringFormat(&error, "Unknown format for %s", path);
		return false;
	}
	
	if(FreeImage_FIFSupportsReading(fif)) {
		bitmap = FreeImage_Load(fif, filename);
	} else {
		S2StringFormat(&error, "File format does no support read.");
		return false;
	}
	if(!dib) {
		bitmap(&error, "Cannot load file %s", path);
		return false;
	}
	
	switch(format) {
	case R8G8B8A8:
		{
			FIBITMAP *bitmap_32 = FreeImage_ConvertTo32Bits(bitmap);
			CHECK(bitmap_32)<<"Cannot convert to 32bit bitmap";
			FreeImage_Unload(bitmap);
			bitmap = bitmap_32;
			CHECK(FreeImage_GetImageType(bitmap) == FIT_INT32);
			unsigned int width = FreeImage_GetWidth(bitmap);
			unsigned int height = FreeImage_GetHeight(bitmap);
			unsigned int pitch = FreeImage_GetPitch(bitmap);
			CHECK(pitch == width*4);
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
	
const void * PixelMap::GetRawMemory() const {
	Check();
	return (const void *)FreeImage_GetBits(bitmap);
}

const s2string & PixelMap::GetPath() const {
	Check();
	return path;
}

PixelMap::Format PixelMap::GetFormat() const {
	Check();
	return format;
}

void PixelMap::Check() {
	CHECK_NOTNULL(bitmap);
}



}

