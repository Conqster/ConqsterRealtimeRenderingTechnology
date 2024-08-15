#version 400

out vec4 FragColour;


in vec3 v_Position;
in vec3 v_Normal;

uniform vec3 u_CamPos;
uniform samplerCube u_SkyboxTex;

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

	float refraction_ratio = 1.00 / 1.52;
	vec3 I = normalize(v_Position - u_CamPos);
    vec3 R = reflect(I, normalize(v_Normal));
    //vec3 R = refract(I, normalize(v_Normal),refraction_ratio);
	FragColour = vec4(texture(u_SkyboxTex, R).rgb, 1.0f);
}