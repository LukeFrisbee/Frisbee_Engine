#version 450

// in
layout (location = 0) in vec3 color;

// out
layout (location = 0) out vec4 outColor;

// push
layout(push_constant) uniform Push {
	mat4 transform;
	mat4 normalMatrix;
} push;

// variables

void main()
{
	outColor = vec4(color, 1.0);
}

