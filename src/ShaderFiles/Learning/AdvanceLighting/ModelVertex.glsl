#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;


out VS_OUT
{
	vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
	vec4 colour;
}vs_out;


layout (std140)  uniform u_CameraMat
{
	mat4 projection;
	mat4 view;
};

uniform mat4 u_Model;


void main()
{
	gl_Position = projection * view * u_Model * pos;
	
	vs_out.texCoords = uv;
	vs_out.fragPos = vec3(u_Model * pos);
	vs_out.normal = nor;
	vs_out.colour = col;
}