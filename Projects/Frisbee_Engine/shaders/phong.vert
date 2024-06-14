#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 fragColor;

// descriptor set
layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionViewMatrix;
	vec3 directionToLight;
} ubo;

// push
layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

// variables
const vec3 AMBIENT = vec3(0.02, 0.02, 0.02);

void main()
{
	gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);

	vec3 normalWorldSpace = normalize(push.normalMatrix * vec4(normal, 0.0)).xyz;
	float lightIntensity = max(dot(normalWorldSpace, ubo.directionToLight), 0);

	vec3 vertexColor = AMBIENT + lightIntensity * vec3(1.0, 1.0, 1.0);
	fragColor = clamp(vertexColor, 0.0, 1.0);
}