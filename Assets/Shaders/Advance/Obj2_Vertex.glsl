#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 nor;

out vec2 v_UVCoord;
out vec4 v_Colour;

layout (std140)  uniform u_CameraMat
{
	mat4 projection;
	mat4 view;
};

uniform mat4 u_Model;

void main()
{
	gl_Position = projection * view * u_Model * pos;
	
	v_UVCoord = uv;
	v_Colour = vec4(0.0f, 1.0f, 0.0f, 1.0f);//col;
	
	//normal is not required for texturing
}