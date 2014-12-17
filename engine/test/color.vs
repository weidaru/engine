struct VertexInputType
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer WorldViewProjection {
	float4x4 world;
	float4x4 view;
	float4x4 projection;

}

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position.xyz = input.position.xyz;
	output.position.w = 1.0f;
	output.position = mul(world, output.position);
	output.position = mul(view, output.position);
	output.position = mul(projection, output.position);
    
	// Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
}