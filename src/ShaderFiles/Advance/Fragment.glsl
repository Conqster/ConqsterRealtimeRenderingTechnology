#version 400

///////
//Layout location qualifer can only be used v4.3 and above
///////
//layout (location = 1) in vec4 col;

in vec2 v_UVCoord;
in vec4 v_Colour;

out vec4 o_Colour;

uniform sampler2D u_Texture;

void main()
{
	vec4 tex_colour = texture(u_Texture, v_UVCoord);
	
	o_Colour = tex_colour;
	
	
	//For debugging
	o_Colour = v_Colour;
	//o_Colour = col;
}