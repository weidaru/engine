struct SimpleMaterialPixelInput {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

struct SimpleMaterialPixelOutput {
	float4 color : SV_TARGET;
};

cbuffer WorldViewProjection {
	float4x4 world;
	float4x4 view;
	float4x4 projection;
}

SimpleMaterialPixelOutput main( SimpleMaterialPixelInput input ) {
	SimpleMaterialPixelOutput output;

	//Assume directional light
	float3 light_dir = float3(0.0f, 0.5f, -1.0f);
	float4 light = float4(1.0f, 1.0f, 1.0f, 1.0f);
	normalize(light_dir);
	
	float n_dot_l = saturate(abs(dot(light_dir, input.normal)));
	
	//Diffuse and ambient. 
	output.color = input.color *light * n_dot_l  + 0.2f * light * input.color;

	return output;
}