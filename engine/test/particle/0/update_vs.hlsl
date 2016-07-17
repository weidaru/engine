struct Input {
	float3 position : POSITION;
};

struct Output {
	float4 position : SV_POSITION;
};


Output main(Input input) {
	Output output;
	output.position.xyz = input.position.xyz;
	output.position.w = 1.0;

	return output;
}