#version 400

layout (location = 0) in vec3 pos;		
layout (location = 1) in vec4 instanceColour; 	
layout (location = 2) in mat4 instanceMatrix;

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
	gl_Position = projection * view * instanceMatrix * vec4(pos, 1.0f);
	//gl_Position = projection * view * vec4(pos, 1.0f);
	o_Colour = instanceColour;
}