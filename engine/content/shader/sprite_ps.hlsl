struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct PixelOutputType {
	float4 rt : SV_TARGET;
};

PixelOutputType main(PixelInputType input) {
	PixelOutputType output;
	output.rt = input.color;
	return output;
}
