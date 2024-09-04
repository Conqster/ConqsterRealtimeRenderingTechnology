#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

out VS_OUT
{
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
	gl_Position = view * u_Model * pos;
	mat3 nor_mat = mat3(transpose(inverse(view * u_Model)));
	vs_out.normal = normalize(vec3(vec4(nor_mat * nor, 0.0f)));
	
	vs_out.colour = col;
}