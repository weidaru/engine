#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")


#include "d3d11_enum_converter.h"

#define WIN32_LEAN_AND_MEAN
#include <dxgi.h>
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

namespace s2 {

DXGI_FORMAT D3D11EnumConverter::TextureFormatToDXGIFormat(TextureEnum::TextureFormat input) {
	switch(input) {
	case TextureEnum::R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case TextureEnum::D24_UNORM_S8_UINT:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case TextureEnum::R32G32B32A32_FLOAT:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	default:
		CHECK(false)<<"Invalid input "<<input;
		return DXGI_FORMAT_FORCE_UINT;
	}
}

TextureEnum::TextureFormat D3D11EnumConverter::DXGIFormatToTextureFormat(DXGI_FORMAT input) {
	switch(input) {
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return TextureEnum::R8G8B8A8_UNORM;
	default:
		CHECK(false)<<"Invalid input "<<input;
		return TextureEnum::R8G8B8A8_UNORM;
	}
}

D3D11_COMPARISON_FUNC D3D11EnumConverter::DSCompareToD3D11Compare(DepthStencilOption::ComparisonFunc input) {
	switch(input) {
	case DepthStencilOption::NEVER:
		return D3D11_COMPARISON_NEVER;
	case DepthStencilOption::ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
	case DepthStencilOption::EQUAL:
		return D3D11_COMPARISON_EQUAL;
	case DepthStencilOption::NOT_EQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;
	case DepthStencilOption::LESS:
		return D3D11_COMPARISON_LESS;
	case DepthStencilOption::LESS_EQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;
	case DepthStencilOption::GREATER:
		return D3D11_COMPARISON_GREATER;
	case DepthStencilOption::GREATER_EQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;
	default:
		CHECK(false)<<"Invalid input"<<input;
		return D3D11_COMPARISON_NEVER;
	}
}

D3D11_STENCIL_OP D3D11EnumConverter::StencilOpToD3D11StencilOp(DepthStencilOption::StencilOp input) {
	switch(input) {
	case DepthStencilOption::KEEP:
		return D3D11_STENCIL_OP_KEEP;
	case DepthStencilOption::ZERO:
		return D3D11_STENCIL_OP_ZERO;
	case DepthStencilOption::REPLACE:
		return D3D11_STENCIL_OP_REPLACE;
	case DepthStencilOption::INC_SAT:
		return D3D11_STENCIL_OP_INCR_SAT;
	case DepthStencilOption::DEC_SAT:
		return D3D11_STENCIL_OP_DECR_SAT;
	case DepthStencilOption::INVERT:
		return D3D11_STENCIL_OP_INVERT;
	case DepthStencilOption::INC:
		return D3D11_STENCIL_OP_INCR;
	case DepthStencilOption::DEC:
		return D3D11_STENCIL_OP_DECR;
	default:
		CHECK(false)<<"Invalid input"<<input;
		return D3D11_STENCIL_OP_KEEP;
	}
}

D3D11_BLEND D3D11EnumConverter::BlendFactorToD3D11Blend(BlendOption::BlendFactor input) {
	switch(input) {
	case BlendOption::ZERO:
		return D3D11_BLEND_ZERO;
	case BlendOption::ONE:
		return D3D11_BLEND_ONE;
	case BlendOption::SRC_COLOR:
		return D3D11_BLEND_SRC_COLOR;
	case BlendOption::INV_SRC_COLOR:
		return D3D11_BLEND_INV_SRC_COLOR;
	case BlendOption::SRC_ALPHA:
		return D3D11_BLEND_SRC_ALPHA;
	case BlendOption::INV_SRC_ALPHA:
		return D3D11_BLEND_INV_SRC_ALPHA;
	case BlendOption::DEST_COLOR:
		return D3D11_BLEND_DEST_COLOR;
	case BlendOption::INV_DEST_COLOR:
		return D3D11_BLEND_INV_DEST_COLOR;
	case BlendOption::DEST_ALPHA:
		return D3D11_BLEND_DEST_ALPHA;
	case BlendOption::INV_DEST_ALPHA:
		return D3D11_BLEND_INV_DEST_ALPHA;
	case BlendOption::BLEND_FACTOR:
		return D3D11_BLEND_BLEND_FACTOR;
	case BlendOption::INV_BLEND_FACTOR:
		return D3D11_BLEND_INV_BLEND_FACTOR;
	default:
		CHECK(false)<<"Invalid input"<<input;
		return D3D11_BLEND_ZERO;
	}
}

D3D11_BLEND_OP D3D11EnumConverter::BlendOpToD3D11BlendOp(BlendOption::BlendOp input) {
	switch(input) {
	case BlendOption::ADD:
		return D3D11_BLEND_OP_ADD;
	case BlendOption::SUBTRACT:
		return D3D11_BLEND_OP_SUBTRACT;
	case BlendOption::REV_SUBTRACT:
		return D3D11_BLEND_OP_REV_SUBTRACT;
	case BlendOption::MIN:
		return D3D11_BLEND_OP_MIN;
	case BlendOption::MAX:
		return D3D11_BLEND_OP_MAX;
	default:
		CHECK(false)<<"Invalid input"<<input;
		return D3D11_BLEND_OP_ADD;
	}
}

D3D_PRIMITIVE_TOPOLOGY D3D11EnumConverter::TopologyToD3D11Topology(GraphicPipeline::PrimitiveTopology input) {
	switch(input) {
	case GraphicPipeline::POINT_LIST:
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case GraphicPipeline::LINE_LIST:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case GraphicPipeline::LINE_STRIP:
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case GraphicPipeline::TRIANGLE_LIST:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case GraphicPipeline::TRIANGLE_STRIP:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	default:
		CHECK(false)<<"Invalid input"<<input;
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
}

}















