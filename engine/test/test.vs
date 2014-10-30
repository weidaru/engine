struct Vertex
{
    float4 Position : POSITION;
    float3 UVPosition : TEXCOORD;
};

struct Pixel
{
    float4 Position : SV_POSITION;
    float2 UVPosition : TEXCOORD;
};

struct FooStruct {
	float3 a;
};

struct MyStruct {
	float  b[3];
	float4x4 a;
	float3 c;
	FooStruct foo;
};

bool myBool;

cbuffer CBuffer{
MyStruct myStruct;
float4x4 worldViewProj;
}

tbuffer MyMatrix {
float4x4 worldViewProj2[10];
};

Buffer<float4> g_BufferBoneWorld;

Texture2D<float4> diffuse   : register(t0);
Texture2D height    : register(t1);
Texture2D<float4> lightmap  : register(t2);
SamplerState pictureSampler;

Pixel PerVertex(Vertex input)
{
    Pixel output = (Pixel) 0;
	if(myBool) {
		input.Position.z += (height.SampleLevel(pictureSampler, float2(0,0), 0).r / 2 + g_BufferBoneWorld.Load(0));
	}
    
    output.Position = mul(input.Position, worldViewProj);
	output.Position = mul(output.Position, worldViewProj2[0]);
    output.UVPosition = input.UVPosition;
    return output;
}

float4 PerPixel(Pixel input) : SV_Target
{
    return diffuse.Sample(pictureSampler, input.UVPosition) * lightmap.Sample(pictureSampler, input.UVPosition);
}