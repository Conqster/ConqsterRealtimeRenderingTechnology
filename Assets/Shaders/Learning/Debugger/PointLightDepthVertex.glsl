#version 400

layout (location = 0) in vec4 pos;

uniform mat4 u_Model;


void main()
{
	gl_Position = u_Model * pos;

}