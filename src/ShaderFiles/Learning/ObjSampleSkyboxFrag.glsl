#version 400

out vec4 FragColour;


in vec3 v_Position;
in vec3 v_Normal;

uniform vec3 u_CamPos;
uniform samplerCube u_SkyboxTex;


void main()
{
	float refraction_ratio = 1.00 / 1.52;
	vec3 I = normalize(v_Position - u_CamPos);
    vec3 R = reflect(I, normalize(v_Normal));
    //vec3 R = refract(I, normalize(v_Normal),refraction_ratio);
	FragColour = vec4(texture(u_SkyboxTex, R).rgb, 1.0f);
}