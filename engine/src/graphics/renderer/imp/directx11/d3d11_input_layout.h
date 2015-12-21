#ifndef D3D11_INPUT_LAYOUT_H_
#define	D3D11_INPUT_LAYOUT_H_

#include "graphics/renderer/input_layout.h"

struct ID3D11InputLayout;

namespace s2 {

class D3D11GraphicResourceManager;

class D3D11InputLayout : public InputLayout {
public:
	D3D11InputLayout(D3D11GraphicResourceManager *_manager);
	
	virtual ~D3D11InputLayout();

	virtual bool InitializeWithElement(const std::vector<ElementDescriptor> &elements, const VertexShader &shader) override;
	virtual bool InitializeWithVertexBuffer(const std::vector<VertexBufferDescriptor> &vbs, const VertexShader &shader) override;

	ID3D11InputLayout *GetInternal() { return layout; }

private:
	D3D11GraphicResourceManager *manager;
	
	ID3D11InputLayout *layout;

};

}


#endif // !D3D11_INPUT_LAYOUT_H_
