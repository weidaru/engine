#ifndef MAPPED_RESOURCE_H_
#define MAPPED_RESOURCE_H_

namespace s2 {

namespace internal {

template <bool enable> 
struct enable_if;

template <>
struct enable_if<true> {
typedef bool you_are_using_the_wrong_writable_if_undefined;
};

}

/**
 * This class serves as similar role as LockGuard to Locks. Use it to safely map and unmap resource.
 * Error check will be processed in compile time.
 *
 * Data should be something the user want to manipulate, such as some C struct.
 * Core is the most import type which should serves as the role of Data manager.
 *	1) Make sure it has default constructor.
 *  2) It must provide syntax compatibility with following signatures:
 *		void * 			Map();
 *		void 			UnMap();
 *		unsigned int 	GetSize();
 *	3) Destructor is required to avoid memory leak.
 * writable determines whether the MappedResource is writable by cpu.
 */
template <typename Data, typename Option, typename Core, bool readable, bool writable>
class MappedResource  {
public:
	MappedResource(Core *new_core) : core(new_core) { }

	const Data * Map() const {
		internal::enable_if<readable>::you_are_using_the_wrong_writable_if_undefined dummy = false;
		return (const Data *)(core->Map());
	}
	
	Data * MapEditable() {
		internal::enable_if<writable>::you_are_using_the_wrong_writable_if_undefined dummy = false;
		return (Data *)core->Map();
	}
	
	void UnMap() {
		core->UnMap();
	}
	
	unsigned int GetSize() {
		return core->GetSize()/sizeof(Data);
	}

private:
	MappedResource(const MappedResource &);
	MappedResource & operator=(const MappedResource &);
	
private:
	Core *core;
};

}

#endif		//MAPPED_RESOURCE_H_