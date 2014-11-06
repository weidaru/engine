struct VertexInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
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

PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position.xyz = input.position.xyz;
	output.position.w = 1.0f;
	output.position = mul(output.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
    
	// Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
}