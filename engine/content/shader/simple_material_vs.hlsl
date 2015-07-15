#include "simple_material_common.hlsl"

cbuffer {
	float4x4 world;
	float4x4 view;
	float4x4 projection;
};

SimpleMaterialPixelInput main(SimpleMaterialVertexInput input) {
	SimpleMaterialPixelInput output;

	output.position = float4(input.position, 1.0);
	output.position = mul(world, output.position);
	output.position = mul(view, output.position);
	output.position = mul(projection, output.position);

	output.color = input.color;
	
	float4 normal_w = float4(normal_w, 0.0f);
	normal_w = mul(world, normal_w);
	
	output.normal = normal_w.xyz;

	return output;
}