#ifndef MAPPABLE_H_
#define MAPPABLE_H_

namespace s2 {

class Mappable {
public:
	virtual void Map(bool is_partial_map) = 0;
	virtual void Update(unsigned int offset, void *data, unsigned int size) = 0;
	virtual void UnMap() = 0;
	
};


}


#endif		//MAPPABLE_H_
