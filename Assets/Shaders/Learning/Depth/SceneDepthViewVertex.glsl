#version 400

layout (location = 0) in vec4 pos;

out vec4 v_Colour;

//--------------uniform--------------/
layout (std140) uniform u_CameraMat
{
	mat4 proj;
	mat4 view;
};


uniform mat4 u_Model;

void main()
{
	gl_Position = (proj * view * u_Model) * pos;
	
}
