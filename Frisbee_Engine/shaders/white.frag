#version 450

// out
layout (location = 0) out vec4 FragColor;

// push
layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

