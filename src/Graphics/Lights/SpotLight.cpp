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

void SpotLight::Use(int colour_location, int ambient_intensity_loc, unsigned int diffuse_intensity_loc, int position_location, int attenuationLocation, int dir_location, int falloff_location)
{
	//TO-DO: just hack for now
	if (m_Disable)
	{
		GLCall(glUniform3f(colour_location, 0.0f, 0.0f, 0.0f));
		return;
	}
	GLCall(glUniform3f(colour_location, m_Colour.x, m_Colour.y, m_Colour.z));
	GLCall(glUniform1f(ambient_intensity_loc, m_AmbientIntensity));
	GLCall(glUniform1f(diffuse_intensity_loc, m_DiffuseIntensity));

	GLCall(glUniform3f(position_location, m_Position.x, m_Position.y, m_Position.z));
	GLCall(glUniform3fv(attenuationLocation, 1, m_AttenuationConstants));

	GLCall(glUniform3f(dir_location, m_Direction.x, m_Direction.y, m_Direction.z));
	GLCall(glUniform1f(falloff_location, m_Falloff));
}
