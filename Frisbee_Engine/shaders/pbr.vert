#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;

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
	vec3 normalWorldSpace = normalize(push.normalMatrix * vec4(normal, 0.0)).xyz;
	outNormal = normalWorldSpace;

	vec4 worldPos = push.modelMatrix * vec4(position, 1.0);
	outPosition = worldPos.xyz;

	gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);;
}