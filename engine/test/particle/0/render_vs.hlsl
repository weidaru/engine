cbuffer constant {
	uint width;
	uint height;
};

struct Output {
	float4 position : SV_POSITION;
};

Texture2D position_tex;

Output main(uint id : SV_VERTEXID ) {
	Output output;
	uint y = id/width, x = id - y*width;
	output.position = position_tex[uint2(x, y)];


	return output;
}