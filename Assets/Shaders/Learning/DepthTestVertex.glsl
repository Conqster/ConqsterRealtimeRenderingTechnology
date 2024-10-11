#version 400

layout(location = 0) in vec4 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 uv;

out vec3 v_FragPos;
out vec2 v_TexCoord;

uniform mat4 u_Projection;
uniform mat4 u_Model;
uniform mat4 u_View;

void main()
{
	gl_Position = (u_Projection * u_View * u_Model) * pos;
	v_FragPos = vec3(u_Model * pos);
	v_TexCoord = uv;
}