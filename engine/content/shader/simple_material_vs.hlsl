cbuffer WorldViewProjection {
	float4x4 world;
	float4x4 view;
	float4x4 projection;
}

struct SimpleMaterialVertexInput {
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

struct SimpleMaterialVertexOutput {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

SimpleMaterialVertexOutput main(SimpleMaterialVertexInput input) {
	SimpleMaterialVertexOutput output;

	output.position = float4(input.position, 1.0);
	output.position = mul(world, output.position);
	output.position = mul(view, output.position);
	output.position = mul(projection, output.position);

	output.position = output.position/output.position.w;
	output.position.w = 1.0f;

	output.color = input.color;
	
	output.normal = mul((float3x3)(world), input.normal);
	normalize(output.normal);

	return output;
}