#version 400

out vec4 FragColour;

in vec2 v_UVCoord;
in vec4 v_Colour;

uniform sampler2D u_ScreenTex;



void main()
{
	FragColour = v_Colour;
	//FragColour = vec4(v_UVCoord, 0.0f, 1.0f);
	FragColour = vec4(texture(u_ScreenTex, v_UVCoord).rgb, 1.0f);

}

