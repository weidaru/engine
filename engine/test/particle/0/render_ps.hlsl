struct Input {
	float4 position : SV_POSITION;
};

float4 main(Input input) : SV_TARGET0 {
	return float4(input.position.x/100.0, input.position.y/200.0, input.position.x/300.0, 1.0);
}