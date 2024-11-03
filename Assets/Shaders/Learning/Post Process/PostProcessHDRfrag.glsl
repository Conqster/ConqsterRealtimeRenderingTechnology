#version 400

out vec4 FragColour;

in vec2 v_UVCoord;
in vec4 v_Colour;

uniform sampler2D u_ScreenTex;
uniform float exposure = 1.0f;

const float gamma = 1.80f;

void main()
{
	vec3 hdr_colour = texture(u_ScreenTex, v_UVCoord).rgb;
	
	//Reinhard
	vec3 mapped = hdr_colour / (hdr_colour + vec3(1.0f));
	mapped = vec3(1.0f) - exp(-hdr_colour * exposure);
	//gamma correction 
	mapped = pow(mapped, vec3(1.0f / gamma));
	FragColour = vec4(mapped, 1.0f);
}

