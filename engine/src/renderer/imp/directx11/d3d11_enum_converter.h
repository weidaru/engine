#ifndef D3D11_ENUM_CONVERTER_H_
#define D3D11_ENUM_CONVERTER_H_

#include "renderer/texture_enum.h"
#include "renderer/depth_stencil_option.h"
#include "renderer/blend_option.h"
#include "renderer/vertex_shader.h"

enum DXGI_FORMAT;
enum D3D11_COMPARISON_FUNC;
enum D3D_PRIMITIVE_TOPOLOGY;
enum D3D11_STENCIL_OP;
enum D3D11_BLEND;
enum D3D11_BLEND_OP;

namespace s2 {

class D3D11EnumConverter {
public:
	static DXGI_FORMAT TextureFormatToDXGIFormat(TextureEnum::TextureFormat input);
	static TextureEnum::TextureFormat DXGIFormatToTextureFormat(DXGI_FORMAT input);
	
	static D3D11_COMPARISON_FUNC DSCompareToD3D11Compare(DepthStencilOption::ComparisonFunc input);
	static D3D11_STENCIL_OP StencilOpToD3D11StencilOp(DepthStencilOption::StencilOp input);
	
	static D3D11_BLEND BlendFactorToD3D11Blend(BlendOption::BlendFactor input);
	static D3D11_BLEND_OP BlendOpToD3D11BlendOp(BlendOption::BlendOp input);

	static D3D_PRIMITIVE_TOPOLOGY TopologyToD3D11Topology(VertexShader::PrimitiveTopology input);
};

}


#endif		//D3D11_ENUM_CONVERTER_H_