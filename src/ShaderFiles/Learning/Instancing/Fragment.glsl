#version 400

out vec4 o_Colour;

in VS_OUT
{
	vec4 colour;
	vec2 tex_coord;
}fs_in;

uniform bool u_Debug;

uniform sampler2D u_Texture;


void main()
{
	o_Colour = (u_Debug) ? vec4(1.0f, 0.0f, 1.0f, 1.0f) : fs_in.colour;
	
	if(!u_Debug)
		o_Colour = texture(u_Texture, fs_in.tex_coord);
}