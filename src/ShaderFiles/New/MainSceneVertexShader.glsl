#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 textureCoord;
layout (location = 3) in vec3 normals;

out vec4 v_Colour;
out vec3 v_Normals;
out vec3 v_FragPos;
out vec2 v_TexCoord;

uniform mat4 u_projection;
uniform mat4 u_model;
uniform mat4 u_view;

void main()
{
	gl_Position = (u_projection * u_view * u_model) * pos;
	//gl_Position = u_model * pos;
	v_Colour = col;
	v_FragPos = vec3(u_model * pos);
	v_Normals = mat3(transpose(inverse(u_model))) * normals;
	v_TexCoord = textureCoord;
}

