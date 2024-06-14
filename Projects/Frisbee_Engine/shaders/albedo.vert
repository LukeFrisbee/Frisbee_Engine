#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout(set = 0, binding = 0) uniform uboGlobal {
	mat4 projectionViewMatrix;
	vec3 lightPos;
	vec3 camPos;
} ubo;

// push
layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;


void main()
{
	gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);
}