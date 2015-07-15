#ifndef SIMPLE_MATERIAL_COMMON_H_
#define SIMPLE_MATERIAL_COMMON_H_

struct SimpleMaterialVertexInput {
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

struct SimpleMaterialPixelInput {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

struct SimpleMaterialPixelOutput {
	float4 color : SV_TARGET;
};

#endif //SIMPLE_MATERIAL_COMMON_H_