#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;

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
	vec3 normalWorldSpace = normalize(push.normalMatrix * vec4(normal, 0.0)).xyz;
	outNormal = normalWorldSpace;

	vec4 worldPos = push.modelMatrix * vec4(position, 1.0);
	outPosition = worldPos.xyz;

	gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);;
}