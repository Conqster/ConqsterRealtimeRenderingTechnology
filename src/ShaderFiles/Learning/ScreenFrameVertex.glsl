#version 400

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uvCoord;


out vec2 v_UVCoord;


void main()
{
	gl_Position = vec4(pos, 0.0f, 1.0f);
	v_UVCoord = uvCoord;
}

