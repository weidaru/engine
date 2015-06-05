struct VertexInputType
{
	float3 position : POSITION;

	float4x4 transform: INSTANCE_TRANSFORM;
	float4 color : INSTANCE_COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct PixelOutputType {
	float4 rt : SV_TARGET;
};

PixelInputType fs_main(VertexInputType input)
{
	PixelInputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position.xyz = input.position.xyz;
	output.position.w = 1.0f;

	output.position = mul(input.transform, output.position);

	// Store the input color for the pixel shader to use.
	output.color = input.color;
	
	return output;
}

PixelOutputType ps_main(PixelInputType input) {
	PixelOutputType output;
	output.rt = input.color;
	return output;
}
