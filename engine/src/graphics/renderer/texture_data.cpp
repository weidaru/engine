#include "texture_data.h"

#include <glog/logging.h>

namespace s2 {

TextureData::TextureData(uint32_t _array_size, uint32_t _mip_level){
	Reset(_array_size, _mip_level);
}

TextureData::TextureData() : TextureData(1, 1) {
}

TextureData & TextureData::Reset(uint32_t _array_size, uint32_t _mip_level) {
	array_size = _array_size;
	CHECK(array_size>0)<<"ArraySize must be greater than zero.";

	mip_level = _mip_level == 0 ? 1 : _mip_level;
	data.resize(_array_size * mip_level, 0);

	return *this;
}

TextureData & TextureData::SetData(uint32_t array_index, uint32_t mip_index, void * d) {
	data[CalculateIndex(array_index, mip_index)] = d;
	return *this;
}

void * TextureData::GetData(uint32_t array_index, uint32_t mip_index) const {
	return data[CalculateIndex(array_index, mip_index)];
}

uint32_t TextureData::GetArraySize() const {
	return array_size;
}

uint32_t TextureData::GetMipLevel() const {
	return mip_level;
}

uint32_t TextureData::GetSize() const {
	return data.size();
}

bool TextureData::IsEmpty() const {
	for(auto it=data.begin(); it!=data.end(); it++) {
		if(*it != 0) {
			return false;
		}
	}
	return true;
}

uint32_t TextureData::CalculateIndex(uint32_t array_index, uint32_t mip_index) const {
	return array_index*mip_level + mip_index;
}


TextureCubeData::TextureCubeData() {
	TextureCubeData(1, 1);
}

TextureCubeData::TextureCubeData(uint32_t _array_size, uint32_t _mip_level) {
	Reset(_array_size, _mip_level);
}

TextureCubeData & TextureCubeData::Reset(uint32_t _array_size, uint32_t _mip_level) {
	array_size = _array_size;
	CHECK(array_size>0)<<"ArraySize must be greater than zero.";

	mip_level = _mip_level == 0 ? 1 : _mip_level;
	data.resize(_array_size * mip_level * 6, 0);

	return *this;
}

TextureCubeData & TextureCubeData::SetData(uint32_t array_index, uint32_t mip_index, CubeFace face, void *d) {
	data[CalculateIndex(array_index, mip_index, face)] = d;
	return *this;
}

void * TextureCubeData::GetData(uint32_t array_index, uint32_t mip_index, CubeFace face) const {
	return data[CalculateIndex(array_index, mip_index, face)];
}

uint32_t TextureCubeData::GetArraySize() const {
	return array_size;
}

uint32_t TextureCubeData::GetMipLevel() const {
	return mip_level;
}

uint32_t TextureCubeData::GetSize() const {
	return data.size();
}

uint32_t TextureCubeData::CalculateIndex(uint32_t array_index, uint32_t mip_index, CubeFace face) const {
	return array_index*mip_level*6 + mip_index*6 + face;
}

bool TextureCubeData::IsEmpty() const {
	for(auto it=data.begin(); it!=data.end(); it++) {
		if(*it != 0) {
			return false;
		}
	}
	return true;
}

}


