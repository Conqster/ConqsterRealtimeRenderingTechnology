#include "SpotLight.h"
#include <iostream>

#include "GL/glew.h"
#include "../Renderer.h"

SpotLight::SpotLight() : PointLight(), m_Direction(glm::vec3(0.0f, -1.0f, 0.0f)), m_Falloff(0.1f)
{
	m_Type = Spot;
	std::cout << "Spot Light Created!!!!!\n";
}

SpotLight::SpotLight(float red, float green, float blue, float ambient_intensity, float diffuse_intensity, glm::vec3 pos):
				PointLight(red, green, blue, ambient_intensity, diffuse_intensity, pos), m_Direction(glm::vec3(0.0f, -1.0f, 0.0f)), m_Falloff(0.1f)
{
	m_Type = Spot;
	std::cout << "Spot Light Created!!!!!\n";
}

SpotLight::SpotLight(float red, float green, float blue, float ambient_intensity, float diffuse_intensity, glm::vec3 pos, glm::vec3 dir, float falloff) :
	PointLight(red, green, blue, ambient_intensity, diffuse_intensity, pos), m_Direction(dir), m_Falloff(falloff)
{
	m_Type = Spot;
	std::cout << "Spot Light Created!!!!!\n";
}

void SpotLight::Use(int enable_loc, int colour_loc, int ambient_intensity_loc, unsigned int diffuse_intensity_loc, int position_loc, int attenuation_loc, int dir_loc, int falloff_loc)
{
	//TO-DO: just hack for now
	GLCall(glUniform1i(enable_loc, m_Disable));
	if (m_Disable)
	{
		//GLCall(glUniform3f(colour_loc, 0.0f, 0.0f, 0.0f));
		return;
	}
	GLCall(glUniform3f(colour_loc, m_Colour.x, m_Colour.y, m_Colour.z));
	GLCall(glUniform1f(ambient_intensity_loc, m_AmbientIntensity));
	GLCall(glUniform1f(diffuse_intensity_loc, m_DiffuseIntensity));

	GLCall(glUniform3f(position_loc, m_Position.x, m_Position.y, m_Position.z));
	GLCall(glUniform3fv(attenuation_loc, 1, m_AttenuationConstants));

	GLCall(glUniform3f(dir_loc, m_Direction.x, m_Direction.y, m_Direction.z));
	GLCall(glUniform1f(falloff_loc, m_Falloff));
}
