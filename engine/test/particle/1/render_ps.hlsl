struct Input {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

SamplerState particle_sampler;
Texture2D particle_texture;

float4 main(Input input) : SV_TARGET0 {
    float4 input_color = input.color * particle_texture.Sample(particle_sampler, input.uv);
    //color premultiplied with alpha 
    float4 output_color = input_color * input_color.a;
    output_color.a = input_color.a;
    return output_color;
}