struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 tex : TEXCOORD;
};

struct PixelOutputType {
	float4 rt : SV_TARGET;
};

Texture2D shader_texture;
SamplerState shader_sampler;

PixelOutputType main(PixelInputType input) {
	PixelOutputType output;
	output.rt = input.color + shader_texture.Sample(shader_sampler, input.tex);
	return output;
}
