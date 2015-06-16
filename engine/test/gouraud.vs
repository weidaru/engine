struct VertexInputType
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer WorldViewProjection {
	float4x4 world;
	float4x4 view;
	float4x4 projection;
}

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

	// Calculate the position
    output.position.xyz = input.position.xyz;
	output.position.w = 1.0f;
	output.position = mul(world, output.position);
	output.position = mul(view, output.position);
	output.position = mul(projection, output.position);
	
	// Normal in view space, no scale factor, os inverse transpose is itself.
	float3 normal_view = mul((float3x3)(world), input.normal);
	normal_view = mul((float3x3)(view), normal_view);
	normal_view = normalize(normal_view);
	
	// Assume directional light
	float4 light_color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float3 light_view = mul((float3x3)(view), float3(0.0f, 0.3f, -0.3f));
	light_view = normalize(light_view);
	
	// Calculate reflection
	float n_dot_light = saturate(dot(light_view, normal_view));
	float3 reflection = n_dot_light*normal_view*2 - light_view;
	
	float light_ambient = 0.1f;
	float light_diffuse = 0.4f;
	float light_specular = 0.5f;
	float smooth = 200;
	
	// Diffuse only for now.
    output.color = light_ambient * light_color;
	output.color += light_diffuse * light_color * n_dot_light;
	output.color += light_specular * light_color * pow(saturate(dot(reflection, float3(0.0f, 0.0f, 1.0f))), smooth);
	saturate(output.color);

    
    return output;
}