#version 400

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 col;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normals;

//v from vertex
out vec4 v_colour;
out vec2 v_texCoord;
out vec3 v_testNormal;
out vec3 v_fragPos;

uniform mat4 u_projection;
uniform mat4 u_model;
uniform mat4 u_view;


void main()
{
	gl_Position = (u_projection * u_view * u_model) * pos;
	v_colour = col;
	v_texCoord = texCoord;
	v_testNormal = vec3(pos);
	//v_testNormal = normals;
	v_testNormal = mat3(transpose(inverse(u_model))) * normals;
	v_fragPos = vec3(u_model * pos);
}