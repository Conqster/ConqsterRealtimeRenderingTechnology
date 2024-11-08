#version 400

out vec4 FragColour;

in vec2 v_UVCoord;
in vec4 v_Colour;

uniform sampler2D u_ScreenTex;
uniform bool u_DoHDR;
uniform float exposure = 1.0f;
uniform float u_Gamma = 2.20f;

void main()
{
	vec3 final_colour = texture(u_ScreenTex, v_UVCoord).rgb;
	
	if(u_DoHDR)
	{
		//Reinhard
		vec3 mapped_col = final_colour / (final_colour + vec3(1.0f));
		mapped_col = vec3(1.0f) - exp(-final_colour * exposure);
		//gamma correction 
		mapped_col = pow(mapped_col, vec3(1.0f / u_Gamma));
		final_colour = mapped_col;
	}
	
	FragColour.rgb = final_colour;
	FragColour.a = 1.0f;
	//FragColour = (final_colour, 1.0f);
	
	
	//debugging
	//FragColour = vec4(v_UVCoord, 0.0f, 1.0f);
	//FragColour = v_Colour;
	
}

