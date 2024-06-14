#version 450

// in
layout (location = 0) in vec3 fragColor;

// out
layout (location = 0) out vec4 outColor;

// push
layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	outColor = vec4(fragColor, 1.0);
}

