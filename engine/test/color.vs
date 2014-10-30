struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = input.position;
    
	// Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
}