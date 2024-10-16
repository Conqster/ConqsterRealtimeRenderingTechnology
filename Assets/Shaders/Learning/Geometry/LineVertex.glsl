#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

layout (std140)  uniform u_CameraMat
{
	mat4 projection;
	mat4 view;
};


out vec4 v_Colour;

out VS_OUT
{
	vec4 colour;
}vs_out;

uniform mat4 u_Model;


void main()
{
	gl_Position = projection * view * u_Model * pos;
	gl_Position = vec4(pos.x, pos.y, 0.0f, 1.0f);
	
	v_Colour = col;
	vs_out.colour = col;
}