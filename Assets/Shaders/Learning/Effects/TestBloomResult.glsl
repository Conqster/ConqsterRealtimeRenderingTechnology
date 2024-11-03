#version 330 core
out vec4 FragColour;

in vec2 v_UVCoord;

uniform sampler2D u_SceneTex;
uniform sampler2D u_BloomBlurTex;
uniform float u_Exposure;

void main()
{
	vec3 combine = texture(u_SceneTex, v_UVCoord).rgb + 
				   texture(u_BloomBlurTex, v_UVCoord).rgb;
				   
	vec3 result = vec3(1.0f) - exp(-combine * u_Exposure);
	result = pow(result, vec3(1.0f/2.2f));
	FragColour = vec4(result, 1.0f);
}