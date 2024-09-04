#version 400

layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;


layout(std140) uniform Camera_Mat
{
	mat4 projection;
	mat4 view;
}

out vec3 v_Position;
out vec3 v_Normal;

out vec2 v_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	v_Normal = mat3(transpose(inverse(u_Model))) * nor;
	v_Position = vec3(u_Model * vec4(pos, 1.0f));
	//gl_Position = (u_Projection * u_View * u_Model) * vec4(v_Position, 1.0f);
	//gl_Position = (u_Projection * u_View * u_Model) * vec4(pos, 1.0f);
	gl_Position = projection * view * u_Model * vec4(pos, 1.0f);
	
	v_TexCoords = uv;
}