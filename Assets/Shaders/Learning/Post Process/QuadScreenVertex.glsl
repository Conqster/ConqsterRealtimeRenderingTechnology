#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;

out vec2 v_UVCoord;
out vec4 v_Colour;


void main()
{
	//scale up by 2.0f as the quad constructed is 1 unit, 
	//as in 0.5f to both side to make it ideal for a unit plane
	gl_Position = vec4(pos.x * 2.0f, pos.y * 2.0f, 0.0, 1.0); 
	v_Colour = col; 
	v_UVCoord = uv;
}

