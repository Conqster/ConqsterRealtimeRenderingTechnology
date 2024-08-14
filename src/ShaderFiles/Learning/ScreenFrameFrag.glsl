#version 400

out vec4 FragColour;

in vec2 v_UVCoord;

uniform sampler2D u_ScreenTex;
uniform vec3 u_TexColour;

void main()
{
	//vec4 result_colour = vec4(u_TexColour, 1.0f) * texture(u_ScreenTex, v_UVCoord);
	
	//////////////////
	//INVERSION
	//////////////////
	vec4 result_colour = vec4(u_TexColour, 1.0f) * vec4(vec3(1.0f - texture(u_ScreenTex, v_UVCoord)), 1.0f);
	
	
	////////////////////
	//GRAYSCALE
	////////////////////
	result_colour = texture(u_ScreenTex, v_UVCoord) * vec4(u_TexColour, 1.0f);
	//float average = (result_colour.r + result_colour.g + result_colour.b) / 3.0f;
	float average = 0.2126 * result_colour.r + 0.7152 * result_colour.g + 0.0722 * result_colour.b;
	result_colour = vec4(vec3(average), 1.0f);
	FragColour = result_colour;
}