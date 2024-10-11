#version 400

layout(location = 0) in vec3 pos;
layout(location = 3) in vec3 nor;
layout(location = 2) in vec2 uv;


out vec2 v_TexCoords;
out vec3 v_Normals;
out vec3 v_FragPos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	v_TexCoords = uv;
	gl_Position = u_Projection * u_View * u_Model * vec4(pos, 1.0f);
	v_FragPos = vec3(u_Model * vec4(pos, 1.0f));
	v_Normals = mat3(transpose(inverse(u_Model))) * nor;
}