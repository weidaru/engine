struct Input {
	float4 position : SV_POSITION;		//Pixel shader SV_POSITION is in screen space.
};

struct Output {
	float4 position : SV_TARGET0;
};

cbuffer constants {
	uint width;
	uint height	;
	float time_delta;
};

Texture2D old_position;
SamplerState simple_sampler;

Output main(Input input)  {
	Output output;

	float2 uv = float2(input.position.x/width, input.position.y/height);
	float4 old_pos = old_position.Sample(simple_sampler,  uv);
	float2 other_uv = uv + float2(0.1, 0.0);
	float4 other_pos = old_position.Sample(simple_sampler, other_uv);
	
	float4 velocity = float4(0.0, -0.1, 0.0, 0.0) * clamp(distance(old_pos, other_pos), 0.1, 2.0);
	
	output.position = old_pos + time_delta * velocity;

	return output;
}