struct Input {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

Texture2D paint_texture;
SamplerState paint_sampler;

float4 main(Input input) : SV_TARGET0 {
	return paint_texture.Sample(paint_sampler, input.tex);
}