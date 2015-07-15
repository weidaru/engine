struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 tex : TEXCOORD;
};

struct PixelOutputType {
	float4 rt : SV_TARGET;
};

TextureCube env_cube;
SamplerState shader_sampler;

PixelOutputType main(PixelInputType input) {
	PixelOutputType output;
	output.rt = env_cube.Sample(shader_sampler, input.tex);
	return output;
}
