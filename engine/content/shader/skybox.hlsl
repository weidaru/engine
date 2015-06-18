struct VertexInputType
{
	float3 position : POSITION;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 tex : TEXCOORD;
};

cbuffer WorldViewProjection {
	float4x4 wvp;
}

PixelInputType vs_main(VertexInputType input)
{
	PixelInputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position.xyz = input.position.xyz;
	output.position.w = 1.0f;
	output.position = mul(wvp, output.position);
	output.position.z = 1.0f;
	
	output.tex = input.position;

	return output;
}

TextureCube env_cube;
SamplerState shader_sampler;

struct PixelOutputType {
	float4 rt : SV_TARGET;
};

PixelOutputType ps_main(PixelInputType input) {
	PixelOutputType output;
	output.rt = env_cube.Sample(shader_sampler, input.tex);
	return output;
}
