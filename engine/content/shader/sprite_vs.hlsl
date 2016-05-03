struct VertexInputType
{
	float4x4 transform: INSTANCE_TRANSFORM;
	float4 color : INSTANCE_COLOR;
	float4x2 tex : INSTANCE_TEX;

	uint vertex_id : SV_VERTEXID;
};

struct VertexOutputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 tex : TEXCOORD;
};

VertexOutputType main(VertexInputType input)
{
	VertexOutputType output;

	// Store the input color for the pixel shader to use.
	output.color = input.color;

	// Store texture coordinate and position
	if(input.vertex_id == 0) {
		output.position = float4(-0.1f, -0.1f, 0.0f, 1.0);
		output.tex = input.tex[0];
	} else if(input.vertex_id == 1) {
		output.position = float4(0.1f, -0.1f, 0.0f, 1.0);
		output.tex = input.tex[1];
	} else if(input.vertex_id == 2) {
		output.position = float4(-0.1f, 0.1f, 0.0f, 1.0);
		output.tex = input.tex[2];
	} else if(input.vertex_id == 3) {
		output.position = float4(0.1f, 0.1f, 0.0f, 1.0);
		output.tex = input.tex[3];
	}
	output.position = mul(input.transform, output.position);
	
	return output;
}
