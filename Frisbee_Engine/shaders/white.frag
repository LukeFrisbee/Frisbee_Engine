#version 450

// out
layout (location = 0) out vec4 FragColor;


// descriptor set
layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionViewMatrix;
	vec3 lightPos;
    vec3 camPos;

    // pbr
    vec3 albedo;
	float metallic;
	float roughness;
	float ao;
} ubo;

// push
layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

