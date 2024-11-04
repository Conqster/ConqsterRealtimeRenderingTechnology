#version 400

out vec4 FragColour;

in vec3 v_TexCoords;

uniform samplerCube u_SkyboxTex;

void main()
{
	FragColour = texture(u_SkyboxTex, v_TexCoords);
}