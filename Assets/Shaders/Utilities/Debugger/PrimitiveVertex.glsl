#version 400

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 col; //just if needed

layout (std140)  uniform u_CameraMat
{
	vec3 viewPos;
	float far;
	mat4 projection;
	mat4 view;
	//mat3 padding;
	//vec4 padding;
};


out VS_OUT
{
	vec3 colour; //vertex point colour
}vs_out;


uniform mat4 u_Model;

void main()
{
	gl_Position = projection * view * u_Model * pos;
	
	vs_out.colour = col.rgb;
}