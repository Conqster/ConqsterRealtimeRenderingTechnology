#version 400

out vec4 FragColour;

in vec4 o_Colour;

void main()
{
	FragColour = o_Colour;
	//FragColour = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}