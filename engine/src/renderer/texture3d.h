#ifndef TEXTURE3D_H_
#define TEXTURE3D_H_

#include "resource.h"
#include "texture_enum.h"
#include "utils/s2string.h"
#include "general_enum.h"

namespace s2 {

class Texture3D : public Resource {
public:
	struct Option {
		unsigned int 	width;
		unsigned int 	height;
		unsigned int 	depth;
		unsigned int 	mip_level;			//0 is full mipmap.
		unsigned int 	array_size;
		TextureEnum::TextureFormat 	format;
		unsigned int 	sample_size;
		GeneralEnum::MapBehavior map_behavior;
		TextureEnum::OutputBind output_bind;
		TextureEnum::InputBind input_bind;
		void *data;
		
		Option();
	};

public:
	virtual ~Texture3D() {}
	virtual void Initialize(const Option &option) = 0;
	virtual const Texture3D::Option & GetOption() const = 0;		
	virtual void Map(bool is_partial_map, unsigned int mip_index, unsigned array_index) = 0;
	virtual void Write(unsigned int row, unsigned int col, unsigned int depth, const void *data, unsigned int size) = 0;
	virtual const void * Read(unsigned int row, unsigned int col, unsigned int depth) const = 0;
	virtual void UnMap() = 0;
	
	virtual void Update(
			unsigned int left, unsigned int right,
			unsigned int top, unsigned int bottom,
			unsigned int front, unsigned int back,
			const void *data) = 0;
};

}

#endif 		//TEXTURE3D_H_