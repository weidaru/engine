#ifndef INPUT_LAYOUT_H_
#define INPUT_LAYOUT_H_

#include <stdint.h>
#include <vector>

#include "utils/s2string.h"
#include "utils/type_info.h"

#include "resource.h"

namespace s2 {

class GraphicsBuffer;
class VertexShader;

struct ElementDescriptor {
	uint32_t vertex_buffer_index;
	uint32_t offset;
};

struct VertexBufferDescriptor {
	uint32_t index;
	s2string type_name;

	template<typename T>
	static VertexBufferDescriptor Create(int vb_index) {
		return {vb_index, TypeInfoManager::GetSingleton()->Get<T>().GetName()};
	}
};

class InputLayout : public Resource {
public:
	virtual ~InputLayout() {}

	virtual bool InitializeWithElement(const std::vector<ElementDescriptor> &elements, const VertexShader &shader) = 0;
	virtual bool InitializeWithVertexBuffer(const std::vector<VertexBufferDescriptor> &vbs, const VertexShader &shader) = 0;

	s2string GetLastError() { return error; }

protected:
	s2string error;
};

}


#endif // !INPUT_LAYOUT_H_
