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

	//Assume directional light, direction (-0.577f, 0.577f, -0.577f), color (1.0f, 1.0f, 1.0f, 1.0f)
	float3 light_dir = float3(-0.577f, 0.577f, -0.577f);
	float4 light = float4(1.0f, 1.0f, 1.0f, 1.0f);
	normalize(light_dir);
	
	float n_dot_l = saturate(dot(light_dir, input.normal));
	
	//Diffuse and ambient. 
	output.color = input.color *light * n_dot_l  + 0.2f * light;

	return output;
}