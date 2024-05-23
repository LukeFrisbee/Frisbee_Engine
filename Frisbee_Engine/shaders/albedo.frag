#version 450

// out
layout (location = 0) out vec4 FragColor;


layout(set = 0, binding = 0) uniform uboGlobal {
	mat4 projectionViewMatrix;
	vec3 lightPos;
	vec3 camPos;
} ubo;

layout(set = 0, binding = 1) uniform uboPBR {
	vec3 albedo;
	float metallic;
	float roughness;
	float ao;
} pbr;

// push
layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

