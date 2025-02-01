#version 400

layout (location = 0) in vec3 pos;		
layout (location = 1) in vec4 colour; 	

layout (std140)  uniform u_CameraMat
{
	vec3 viewPos;
	float far;
	mat4 projection;
	mat4 view;
};

//later could have start & end colour;
out vec4 o_Colour;

void main()
{
	gl_Position = projection * view * vec4(pos, 1.0f);
	o_Colour = colour;
}