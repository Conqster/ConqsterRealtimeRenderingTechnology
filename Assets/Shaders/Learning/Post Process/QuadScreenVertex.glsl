#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;

out vec2 v_UVCoord;
out vec4 v_Colour;


void main()
{
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0); 
	v_Colour = col; 
	v_UVCoord = uv;
}

