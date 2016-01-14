cbuffer Transform {
	float4x4 transform;
};

struct Input {
	float3 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct Output {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Output main(Input input) {
	Output output;
	output.pos.xyz = input.pos.xyz;
	output.pos.w = 1.0;
	output.pos = mul(transform, output.pos);

	output.tex = input.tex;

	return output;
}