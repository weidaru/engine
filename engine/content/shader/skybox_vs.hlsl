struct VertexInputType
{
	float3 position : POSITION;
};

struct VertexOutputType
{
	float4 position : SV_POSITION;
	float3 tex : TEXCOORD;
};

cbuffer WorldViewProjection {
	float4x4 wvp;
}

VertexOutputType main(VertexInputType input)
{
	VertexOutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position.xyz = input.position.xyz;
	output.position.w = 1.0f;
	output.position = mul(wvp, output.position);
	output.position.z = 1.0f;
	
	output.tex = input.position;

	return output;
}