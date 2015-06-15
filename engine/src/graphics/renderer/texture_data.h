#ifndef TEXTURE_DATA_H_
#define TEXTURE_DATA_H_

#include <stdint.h>
#include <vector>

namespace s2 {

class TextureData {
public:
	TextureData();
	TextureData(uint32_t array_size, uint32_t mip_level);

	TextureData & Reset(uint32_t array_size, uint32_t mip_level);

	TextureData & SetData(uint32_t array_index, uint32_t mip_index, void *d);
	void * GetData(uint32_t array_index, uint32_t mip_index) const;

	uint32_t GetArraySize() const;
	uint32_t GetMipLevel() const;
	uint32_t GetSize() const;

	uint32_t CalculateIndex(uint32_t array_index, uint32_t mip_index) const;

	bool IsEmpty() const;

private:
	uint32_t array_size;
	uint32_t mip_level;

	std::vector<void *> data;
};

enum CubeFace {
		LEFT, RIGHT, UP, DOWN, NORTH ,SOUTH
};

class TextureCubeData {
public:
	TextureCubeData();
	TextureCubeData(uint32_t array_size, uint32_t mip_level);

	TextureCubeData & Reset(uint32_t array_size, uint32_t mip_level);

	TextureCubeData & SetData(uint32_t array_index, uint32_t mip_index, CubeFace face, void *d);
	void * GetData(uint32_t array_index, uint32_t mip_index, CubeFace face) const;

	uint32_t GetArraySize() const;
	uint32_t GetMipLevel() const;
	uint32_t GetSize() const;

	uint32_t CalculateIndex(uint32_t array_index, uint32_t mip_index, CubeFace face) const;

	bool IsEmpty() const;

private:
	uint32_t array_size;
	uint32_t mip_level;

	std::vector<void *> data;
};

}



#endif		//TEXTURE_DATA_H_