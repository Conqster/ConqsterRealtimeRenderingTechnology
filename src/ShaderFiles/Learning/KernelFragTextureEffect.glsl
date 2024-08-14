#version 330

const int TYPE_BLUR = 0;
const int TYPE_EGDE_DETECTION = 1;

out vec4 FragColour;

in vec2 v_UVCoord;

uniform sampler2D u_ScreenTex;
uniform vec3 u_TexColour;

//const float offset = 1.0f/300.0f;
uniform float u_Offset = 300.0f;

uniform bool u_DoSpecial;
uniform int u_KernelType;

void main()
{
	float offset = 1.0f / u_Offset;
	
	vec2 offsets[9] = vec2[](
	
		vec2(-offset, offset), //top-left
		vec2( 0.0f,   offset), //top-center
		vec2( offset, offset), //top-right
		
		vec2(-offset, 0.0f),   //center-left
		vec2(0.0f, 	  0.0f),   //center-center
		vec2( offset, 0.0f),   //center-right
		
		vec2(-offset, -offset),//bottom-left
		vec2( 0.0f,   -offset),//bottom-center	
		vec2( offset, -offset) //bottom-right
	);
	
	float kernel[9] = float[](
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	);
	
	
	
	if(u_DoSpecial)
	{
		if(u_KernelType == TYPE_BLUR)
		{
			kernel[0] = 1.0f/16;
			kernel[1] = 2.0f/16;
			kernel[2] = 1.0f/16;
	
			kernel[3] = 2.0f/16;
			kernel[4] = 4.0f/16;
			kernel[5] = 2.0f/16;
		
			kernel[6] = 1.0f/16;
			kernel[7] = 2.0f/16;
			kernel[8] = 1.0f/16;
		}
		else if(u_KernelType == TYPE_EGDE_DETECTION)
		{
			kernel[0] = 1.0f;
			kernel[1] = 1.0f;
			kernel[2] = 1.0f;
	
			kernel[3] = 1.0f;
			kernel[4] = -8.0f;
			kernel[5] = 1.0f;
		
			kernel[6] = 1.0f;
			kernel[7] = 1.0f;
			kernel[8] = 1.0f;
		}

	}
	
	
	vec3 sample_Tex[9];
	for(int i = 0; i < 9; i++)
		sample_Tex[i] = vec3(texture(u_ScreenTex, v_UVCoord.st + offsets[i])) * u_TexColour; //Added "u_TexColour" to play around
		
	vec3 colour = vec3(0.0f);
	for(int i = 0; i < 9; i++)
		colour += sample_Tex[i] * kernel[i];
		
	FragColour = vec4(colour, 1.0f);
}
