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
	
	
}

void PixelMap::Clear() {
	delete bitmap;
	bitmap = 0;
	
}
	
const void * PixelMap::GetRawMemory() const {
	Check();
	return 0;
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

