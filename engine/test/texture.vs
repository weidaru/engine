struct VertexInputType {
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType {
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType main(VertexInputType input) {
    PixelInputType output;

    output.position.xyz = input.position.xyz;
	output.position.w = 1.0f;
    output.tex = input.tex;
    
    return output;
}