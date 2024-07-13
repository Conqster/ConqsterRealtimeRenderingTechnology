#include "DirectionalLight.h"

#include <iostream>

#include "GL/glew.h"
#include "../Renderer.h"

DirectionalLight::DirectionalLight() : Light(1.0f, 1.0f, 1.0f, 0.3f, 0.6f), m_Direction(glm::vec3(-1.0f, 1.0f, 1.0f))
{
	m_Type = Directional;
	std::cout << "Directional Light Created!!!!!\n";
}




DirectionalLight::DirectionalLight(float red, float green, float blue, float ambient_intensity, float diffuse_intensity, glm::vec3 dir) :
							Light(red, green, blue, ambient_intensity, diffuse_intensity), m_Direction(dir)
{
	m_Type = Directional;
	std::cout << "Directional Light Created!!!!!\n";
}

void DirectionalLight::Use(int colour_location, int ambient_intensity_loc, unsigned int diffuse_intensity_loc, int direction_location)
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

	GLCall(glUniform3f(direction_location, m_Direction.x, m_Direction.y, m_Direction.z));
}



LightType DirectionalLight::GetType() const
{
	return Directional;
}

DirectionalLight::~DirectionalLight()
{
	std::cout << "Directional Light destroied\n";
}

