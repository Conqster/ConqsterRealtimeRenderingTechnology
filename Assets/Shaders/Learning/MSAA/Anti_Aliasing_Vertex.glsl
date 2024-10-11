#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

out vec4 v_Fragcolour;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * pos;
}