#version 400

in vec4 f_Colour;
in vec4 v_Colour;
out vec4 o_Colour;

uniform bool u_Debug;

void main()
{
	
	o_Colour = v_Colour;
	
	o_Colour = f_Colour;
	
	if(u_Debug)
		o_Colour = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}