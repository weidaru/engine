#include "skybox_image.h"

#include <glog/logging.h>

#include "FreeImage.h"

namespace s2 {

SkyboxImage::SkyboxImage() : is_initialized(false) {
	data.resize(6, 0);
}

SkyboxImage::~SkyboxImage() {
	for(auto it=data.begin(); it!=data.end(); it++) {
		FreeImage_Unload(*it);
	}
}

void SkyboxImage::Check() const {
	CHECK(is_initialized) << "SkyboxImage is not initialized.";
}

bool SkyboxImage::Initialize(const s2string &_path, ImagePixelFormat _format) {
	CHECK(is_initialized == false) << "Cannot initialize twice.";

	path = _path;
	format = _format;
	
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType(path.c_str(), 0);
	if(fif == FIF_UNKNOWN) {
		fif = FreeImage_GetFIFFromFilename(path.c_str());
	}
	if(fif == FIF_UNKNOWN) {
		S2StringFormat(&error, "Unknown format for %s", path.c_str());
		return false;
	}
	
	FIBITMAP *bitmap = 0;
	if(FreeImage_FIFSupportsReading(fif)) {
		bitmap = FreeImage_Load(fif, path.c_str());
	} else {
		S2StringFormat(&error, "File format does no support read.");
		return false;
	}
	if(!bitmap) {
		S2StringFormat(&error, "Cannot load path %s", path.c_str());
		return false;
	}

	switch(format) { 
	case ImagePixelFormat::R8G8B8A8:
		{
			FREE_IMAGE_TYPE type = FreeImage_GetImageType(bitmap);
			uint32_t bpp = FreeImage_GetBPP(bitmap);
			if(type!=FIT_BITMAP || bpp!=32) {
				FIBITMAP *bitmap_32 = FreeImage_ConvertTo32Bits(bitmap);
				FreeImage_Unload(bitmap);
				if(bitmap_32 == 0) {
					S2StringFormat(&error, "Cannot convert to 32bit bitmap of path %s", path.c_str());
					return false;
				}
				bitmap = bitmap_32;
			}
		}
		break;
	default:
		S2StringFormat(&error, "Unsupported format.");
		return false;
		break;
	}

	for(unsigned int i=0; i<6; i++) {
		if(CopySubImage(bitmap, static_cast<Face>(i)) == false) {
			FreeImage_Unload(bitmap);
			return false;
		}
	}

	FreeImage_Unload(bitmap);
	is_initialized = true;

	return true;
}

bool SkyboxImage::CopySubImage(FIBITMAP * source, Face face) {
	static uint32_t offsets[][2] = {{1, 0}, {1, 2}, {0, 1}, {2, 1}, {1, 3}, {1, 1}};

	unsigned int source_width = FreeImage_GetWidth(source);
	unsigned int source_height = FreeImage_GetHeight(source);
	unsigned int width = source_width/4;
	unsigned int height = source_height/3;
	unsigned int left = offsets[face][1]*height, right = (offsets[face][1]+1)*height;
	unsigned int top = offsets[face][0]*width, bottom = (offsets[face][0]+1)*width;
	
	FIBITMAP *sub = FreeImage_Copy(source, left, top, right ,bottom);
	if(sub == 0) {
		S2StringFormat(&error, "Cannot copy sub image for face %d of path %s", face, path.c_str());
		return false;
	}
	if(data[face]) {
		FreeImage_Unload(data[face]);
	}

	if(face == UP) {
		FreeImage_FlipHorizontal(sub);
	} else {
		FreeImage_FlipVertical(sub);
	}
	
	//Flip ARGB to ABGR
	uint32_t scanline = FreeImage_GetPitch(sub)/4;
	uint32_t *raw = (uint32_t *)FreeImage_GetBits(sub);
	for(unsigned int i=0; i<width; i++) {
		for(unsigned int j=0; j<height; j++) {
			uint32_t *address = raw+scanline*j+i;
			uint32_t old = *address;
			uint32_t rep = (old & 0x00FF0000) >> 16;
			rep |= ((old&0x000000FF) << 16);
			*address = (old & 0xFF00FF00) | rep;
		}
	}

	data[face] = sub;

	return true;
}

void * SkyboxImage::GetFaceData(Face face) {
	Check();
	return static_cast<void *>(FreeImage_GetBits(data[face]));
}
	
uint32_t SkyboxImage::GetWidth() const {
	Check();
	return FreeImage_GetWidth(data[0]);
}

uint32_t SkyboxImage::GetHeight() const {
	Check();
	return FreeImage_GetHeight(data[0]);
}

ImagePixelFormat SkyboxImage::GetFormat() const {
	Check();
	return format;
}

void SkyboxImage::PopulateTextureCubeOption(TextureCube::Option *option) {
	option->width = GetWidth();
	option->height = GetHeight();
	switch(format) {
	case ImagePixelFormat::R8G8B8A8:
		option->format = RendererEnum::R8G8B8A8_UNORM;
		break;
	default :
		CHECK(false)<<"Unsupported format "<<static_cast<int>(format);
		break;
	}
	
	option->mip_level = 1;
	option->array_size = 1;
	option->data.Reset(1, 1);
	
	for(unsigned int i=0; i<6; i++) {
		option->data.SetData(0,0, static_cast<CubeFace>(i), GetFaceData(static_cast<Face>(i)));
	}
}

}

