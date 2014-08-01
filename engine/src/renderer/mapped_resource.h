#ifndef MAPPED_RESOURCE_H_
#define MAPPED_RESOURCE_H_

namespace s2 {

namespace internal {

template <bool enable> 
struct enable_if;

template <>
struct enable_if<true> {
typedef bool defined_onlyif_true;
};

}

/**
 * Data should be some pure C struct. Data as void will be specialized see below.
 * Base is the MappedResource will inherit from and it must have default constructor.
 * Core is the most import type which should stick to the following conventions.
 *	1) Make sure it has default constructor.
 *  2) It must provide syntax compatibility with following signatures:
 *		void 	Initialize(unsigned int size, const void *data);
 *		void * 	Map();
 *		void 	UnMap();
 *	3) Destructor is required to avoid memory leak.
 */
template <typename Data, typename Base, typename Core, bool writable>
class MappedResource : public Base  {
public:
	/**
	 * Buffer with exact size of sizeof(T) 
	 */
	MappedResource(const Data *data) {
		core.Initialize(sizeof(Data), data);
	}

	const Data * Map() const {
		return (const Data *)(core.Map());
	}
	
	Data * MapEditable() {
		internal::enable_if<writable>::defined_onlyif_true dummy = false;
		return (Data *)core.Map();
	}
	
	void UnMap() {
		core.UnMap();
	}

private:
	MappedResource(const MappedResource &);
	MappedResource & operator=(const MappedResource &);
	
private:
	Core core;
};

/**
 * Template specialization for non-structured blob data, such as vertex buffer or texture.
 */
template <typename Base, typename Core, bool writable>
class MappedResource <void, Base, Core, writable> : public Base {
public:

	MappedResource(const void *data, unsigned int size) {
		core.Initialize(size, data);
	}

	const void * Map() const {
		return (const void *)core.Map();
	}
	
	void * MapEditable() {
		internal::enable_if<writable>::defined_onlyif_true dummy = false;
		return core.Map();
	}
	
	void UnMap() {
		core.UnMap();
	}
	
	bool GetWritable() { return writable; }

private:
	MappedResource(const MappedResource &);
	MappedResource & operator=(const MappedResource &);
	
private:
	Core core;
};

}

#endif		//MAPPED_RESOURCE_H_