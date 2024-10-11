#version 400

out vec4 o_Colour;

uniform vec4 u_stencilColour;

void main()
{
	o_Colour = u_stencilColour;
}
