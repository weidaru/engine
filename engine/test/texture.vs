struct VertexInputType {
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType {
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

cbuffer WorldViewProjection {
	float4x4 world;
	float4x4 view;
	float4x4 projection;
}

PixelInputType main(VertexInputType input) {
    PixelInputType output;

    output.position.xyz = input.position.xyz;
	output.position.w = 1.0f;
	output.position = mul(world, output.position);
	output.position = mul(view, output.position);
	output.position = mul(projection, output.position);
    
    output.tex = input.tex;
    
    return output;
}