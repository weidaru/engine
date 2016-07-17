#include "common.hlsli"

struct Output {
	float4 position : SV_Position;
    float4 color : COLOR;
    float2 dimension : TEXCOORD;
};

struct Input {
	uint x : INDEX_X;
	uint y : INDEX_Y;
};


Output main(Input input) {
	Output output;
	
	Particle p = getParticle(uint2(input.x, input.y));

    output.position = p.position;
    output.color = p.color;
    output.dimension = p.dimension;

	return output;
}