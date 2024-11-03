#version 400
//#version 330 core
layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BrightColour;
layout (location = 2) out vec4 BaseColour;

in VS_OUT
{
	vec3 fragPos;
	vec2 UVs;
	vec4 fragPosLightSpace;
	mat3 TBN;
	vec3 normal;
}fs_in;


uniform float rate;
uniform vec3 u_Colour;


void main()
{	
	FragColour = vec4(u_Colour, 1.0f);
	
	/////////////
	//Bright Colour
	/////////////
	float brightness = dot(FragColour.rgb, vec3(0.2126, 0.7152, 0.0722) * rate);
	BrightColour = (brightness > 1.0f) ? FragColour : vec4(vec3(0.0f), 1.0f);

	/////////////
	//Base Colour
	/////////////
	BaseColour = vec4(fs_in.fragPos, 1.0f);
}