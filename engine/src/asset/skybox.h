#ifndef	SKYBOX_H_
#define SKYBOX_H_

#include "utils/s2string.h"
#include "image_common.h"

#include "graphics/renderer/texture_cube.h"

#include <stdint.h>
#include <vector>

struct FIBITMAP;

namespace s2 {

class Skybox {
public:
	enum Face {
		LEFT, RIGHT, UP, DOWN, NORTH ,SOUTH
	};

public:
	Skybox();
	~Skybox();

	bool Initialize(const s2string &path, ImagePixelFormat format);
	const s2string &GetLastError() { return error;}

	void * GetFaceData(Face face);
	
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	ImagePixelFormat GetFormat() const;

	void PopulateTextureCubeOption(TextureCube::Option *option);

private:
	void Check() const;
	bool CopySubImage(FIBITMAP * source, Face face);

private:
	s2string path;
	s2string error;

	ImagePixelFormat format;
	std::vector<FIBITMAP *> data;
	bool is_initialized;
};

}

#endif		//SKYBOX_H_