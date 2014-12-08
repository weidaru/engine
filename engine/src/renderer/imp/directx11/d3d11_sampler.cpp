#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include "d3d11_sampler.h"
#include "d3d11_graphic_resource_manager.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#undef ERROR

#include <glog/logging.h>

#include <float.h>

namespace s2 {

namespace {


D3D11_COMPARISON_FUNC SamplerCompareToD3D11Compare(Sampler::ComparisonFunc input) {
	switch(input) {
	case Sampler::NEVER:
		return D3D11_COMPARISON_NEVER;
	case Sampler::ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
	case Sampler::EQUAL:
		return D3D11_COMPARISON_EQUAL;
	case Sampler::NOT_EQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;
	case Sampler::LESS:
		return D3D11_COMPARISON_LESS;
	case Sampler::LESS_EQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;
	case Sampler::GREATER:
		return D3D11_COMPARISON_GREATER;
	case Sampler::GREATER_EQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;
	default:
		CHECK(false)<<"Invalid input"<<input;
		return D3D11_COMPARISON_NEVER;
	}
}

D3D11_TEXTURE_ADDRESS_MODE SamplerWrapToD3D11TextureAddressMode(Sampler::WrapMode input) {
	switch(input) {
	case Sampler::REPEAT:
		return D3D11_TEXTURE_ADDRESS_WRAP;
	case Sampler::CLAMP:
		return D3D11_TEXTURE_ADDRESS_CLAMP;
	case Sampler::MIRROR:
		return D3D11_TEXTURE_ADDRESS_MIRROR;
	case Sampler::BORDER:
		return D3D11_TEXTURE_ADDRESS_BORDER;
	default:
		CHECK(false)<<"Invalid input"<<input;
			return D3D11_TEXTURE_ADDRESS_WRAP;
	}
}

}

D3D11Sampler::D3D11Sampler(D3D11GraphicResourceManager *_manager)
		: manager(_manager), sampler(0){
	CHECK_NOTNULL(manager);
}

D3D11Sampler::~D3D11Sampler() {
	if(sampler) {
		sampler->Release();
		sampler = 0;
	}
}

void D3D11Sampler::Initialize(const Option &_option) {
	option = _option;
	D3D11_SAMPLER_DESC desc;

	if(option.max_anisotropy > 0) {
		if(_option.compare_func == UNDEFINED) {
			desc.MaxAnisotropy = _option.max_anisotropy;
			desc.Filter = D3D11_FILTER_ANISOTROPIC;
		} else {
			desc.MaxAnisotropy = _option.max_anisotropy;
			desc.Filter= D3D11_FILTER_COMPARISON_ANISOTROPIC;
		}
	} else {
		desc.MaxAnisotropy = 1;
		if(option.min_mag_filter == NEAREST) {
			if(option.mip_filter == NEAREST) {
				if(option.compare_func == UNDEFINED) {
					desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT ;
				} else {
					desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
				}
			} else if(option.mip_filter == LINEAR) {
				if(option.compare_func == UNDEFINED) {
					desc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR ;
				} else {
					desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
				}
			} else {
				CHECK(false) << "Unknown mip_filter "<<option.mip_filter;
			}
		} else if (option.min_mag_filter == LINEAR) {
			if(option.mip_filter == NEAREST) {
				if(option.compare_func == UNDEFINED) {
					desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT ;
				} else {
					desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
				}
			} else if(option.mip_filter == LINEAR) {
				if(option.compare_func == UNDEFINED) {
					desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR ;
				} else {
					desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
				}
			} else {
				CHECK(false) << "Unknown mip_filter "<<option.mip_filter;
			}
		} else {
			CHECK(false)<< "Unknown min_mag_filter "<<option.min_mag_filter;
		}
	}
	
	desc.AddressU = SamplerWrapToD3D11TextureAddressMode(option.u_wrap);
	desc.AddressV = SamplerWrapToD3D11TextureAddressMode(option.v_wrap);
	desc.AddressW = SamplerWrapToD3D11TextureAddressMode(option.w_wrap);
	desc.MipLODBias = 0;
	desc.ComparisonFunc = SamplerCompareToD3D11Compare(option.compare_func);
	desc.BorderColor[0] = option.border_color[0];
	desc.BorderColor[1] = option.border_color[1];
	desc.BorderColor[2] = option.border_color[2];
	desc.BorderColor[3] = option.border_color[3];
	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;
	
	HRESULT result= 1;
	result = manager->GetDevice()->CreateSamplerState(&desc, &sampler);
	CHECK(!FAILED(result))<<"Cannot create sampler state";
}

const Sampler::Option & D3D11Sampler::GetOption() const {
	CHECK(sampler)<<"Sampler is not initialized.";
	return option;
}


}

