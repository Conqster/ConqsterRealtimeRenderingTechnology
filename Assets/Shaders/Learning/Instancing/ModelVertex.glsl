#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

layout (std140)  uniform u_CameraMat
{
	mat4 projection;
	mat4 view;
	vec2 screen_res;
};

out VS_OUT
{
	vec4 colour;
	vec2 tex_coord;
}vs_out;

uniform mat4 u_Model;

void main()
{
	gl_Position = projection * view * u_Model * pos;
	
	vs_out.colour = col;
	vs_out.tex_coord = uv;
}