#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 fragColor;

// push
layout(push_constant) uniform Push {
	mat4 transform;
	mat4 normalMatrix;
} push;

// variables
const vec3 DIR_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const vec3 AMBIENT = vec3(0.02, 0.02, 0.02);

void main()
{
	gl_Position = push.transform * vec4(position, 1.0);

	vec3 normalWorldSpace = normalize(push.normalMatrix * vec4(normal, 0.0)).xyz;
	float lightIntensity = max(dot(normalWorldSpace, DIR_TO_LIGHT), 0);

	vec3 vertexColor = AMBIENT + lightIntensity * vec3(1.0, 1.0, 1.0);
	fragColor = vertexColor; clamp(vertexColor, 0.0, 1.0);
}