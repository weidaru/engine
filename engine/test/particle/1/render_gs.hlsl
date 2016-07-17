struct Input {
	float4 position : SV_Position;
    float4 color : COLOR;
    float2 dimension : TEXCOORD;
};

struct Output {
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

//In NDC space
static const float4 kTopLeftOffset = float4(-1.0f, 1.0f, 0.0f, 0.0f);
static const float4 kTopRightOffset = float4(1.0f, 1.0f, 0.0f, 0.0f);
static const float4 kBottomRightOffset = float4(1.0f, -1.0f, 0.0f, 0.0f);
static const float4 kBottomLeftOffset = float4(-1.0f, -1.0f, 0.0f, 0.0f);

[maxvertexcount(4)]
void main(point Input input[1], inout TriangleStream<Output> output_stream) {
    Input center = input[0];
    float4 dimension = float4(center.dimension[0], center.dimension[1], 0.0f, 0.0f);

    Output output;
    float4 bottom_left = kBottomLeftOffset * dimension + center.position;
    output.position = bottom_left;
    output.color = center.color;
    output.uv = float2(0.0f, 0.0f);
    output_stream.Append(output);

    float4 bottom_right = kBottomRightOffset * dimension + center.position;
    output.position = bottom_right;
    output.color = center.color;
    output.uv = float2(1.0f, 0.0f);
    output_stream.Append(output);

    float4 top_left = kTopLeftOffset * dimension + center.position;
    output.position = top_left;
    output.color = center.color;
    output.uv = float2(0.0f, 1.0f);
    output_stream.Append(output);

    float4 top_right = kTopRightOffset * dimension + center.position;
    output.position = top_right;
    output.color = center.color;
    output.uv = float2(1.0f, 1.0f);
    output_stream.Append(output);
}