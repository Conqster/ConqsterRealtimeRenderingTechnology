#version 430 core

out vec4 FragColour;


in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoords;

uniform vec3 u_CamPos;

layout(binding = 0) uniform samplerCube u_SkyboxTex;
layout(binding = 1) uniform sampler2D u_Texture;

uniform float u_SkyboxInfluencity;
uniform vec3 u_TextureColour;



uniform bool u_DoDepthTest;
uniform float u_Near;
float far = 150.0f;


float LinearizeDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * u_Near * far) /(far + u_Near - z * (far - u_Near));
}

void main()
{

	if(u_DoDepthTest)
	{
		//playing with frag z-buffer/ depth
	
		float linear_depth = LinearizeDepth(gl_FragCoord.z) /far;
	
		FragColour = vec4(vec3(linear_depth), 1.0f);
		return;
	}
	
	vec4 result_texture_o = vec4(u_TextureColour, 1.0f);
	
	//result_texture_o = texture(u_Texture, v_TexCoords);
	//result_texture_o = texture(u_Texture, v_TexCoords);
	
	result_texture_o *= texture(u_Texture, v_TexCoords);

	float refraction_ratio = 1.00 / 1.52;
	vec3 I = normalize(v_Position - u_CamPos);
    vec3 R = reflect(I, normalize(v_Normal));
    //vec3 R = refract(I, normalize(v_Normal),refraction_ratio);
	FragColour = result_texture_o + (vec4(texture(u_SkyboxTex, R).rgb, 1.0f) * u_SkyboxInfluencity);
	
	
	////////////////////
	//OpenGL Programming guide (skybox reflection)
	////////////////////
	//FragColour = vec4(0.3f, 0.2f, 0.1f, 1.0f) + vec4(0.97f, 0.83f, 0.79, 0.0f) * (texture(u_SkyboxTex, R) * u_SkyboxInfluencity);
	FragColour = vec4(texture(u_SkyboxTex, R).rgb, 1.0f);
	//FragColour = mix(result_texture_o, (vec4(texture(u_SkyboxTex, R).rgb, 1.0f), 0.2f);
	//FragColour = result_texture_o;
}