#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 bounds;

out vec2 coords;

void main()
{
	coords = bounds;

	gl_Position = vec4(position, 0.0, 1.0);
}