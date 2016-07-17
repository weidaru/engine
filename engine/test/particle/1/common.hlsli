#ifndef COMMON_H_
#define COMMON_H_

Texture2D position_tex;
Texture2D dimension_tex;
Texture2D velocity_tex;
Texture2D acceleration_tex;
Texture2D color_tex;


struct Particle {
	float4 position;
	float4 velocity;
	float4 acceleration;
    float4 color;
    float2 dimension;
};


Particle getParticle(uint2 index) {
    Particle output;

	output.position = position_tex[index];
	output.velocity = velocity_tex[index];
	output.acceleration = acceleration_tex[index];
    output.color = color_tex[index];
    output.dimension = dimension_tex[index].xy;

	return output;
}

#endif