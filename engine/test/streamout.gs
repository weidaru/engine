struct GSInput {
	float4 pos : SV_POSITION;
};

//In NDC space
static const float4 kTopLeftOffset = float4(-0.1f, 0.1f, 0.0f, 0.0f);
static const float4 kTopRightOffset = float4(0.1f, 0.1f, 0.0f, 0.0f);
static const float4 kBottomRightOffset = float4(0.1f, -0.1f, 0.0f, 0.0f);
static const float4 kBottomLeftOffset = float4(-0.1f, -0.1f, 0.0f, 0.0f);

[maxvertexcount(8)]
void main(point GSInput input[1] , inout PointStream<GSInput> output_stream, inout PointStream<GSInput> streamout ) {
	float4 top_left = kTopLeftOffset + input[0].pos;
	float4 top_right = kTopRightOffset + input[0].pos;
	float4 bottom_right = kBottomRightOffset + input[0].pos;
	float4 bottom_left = kBottomLeftOffset + input[0].pos;
	GSInput output;
	output.pos = bottom_left;
	output_stream.Append(output);
	streamout.Append(output);
	output.pos = bottom_right;
	output_stream.Append(output);
	streamout.Append(output);
	output.pos = top_left;
	output_stream.Append(output);
	streamout.Append(output);
	output.pos = top_right;
	output_stream.Append(output);
	streamout.Append(output);
}
