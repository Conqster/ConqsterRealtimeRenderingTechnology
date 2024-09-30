#version 400

out vec4 FragColour;

uniform vec3 u_Colour; 

void main()
{
	FragColour = vec4(u_Colour, 1.0f);
}