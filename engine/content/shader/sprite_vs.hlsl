struct VertexInputType
{
	float3 position : POSITION;

	float4x4 transform: INSTANCE_TRANSFORM;
	float4 color : INSTANCE_COLOR;
};

struct VertexOutputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VertexOutputType main(VertexInputType input)
{
	VertexOutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position.xyz = input.position.xyz;
	output.position.w = 1.0f;

	output.position = mul(input.transform, output.position);

	// Store the input color for the pixel shader to use.
	output.color = input.color;
	
	return output;
}
