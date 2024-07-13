#include "Light.h"
#include <iostream>

Light::Light() : m_Colour(glm::vec3(1.0f, 0.0f, 1.0f)),
				m_AmbientIntensity(0.5f), m_DiffuseIntensity(0.6f), m_Disable(false)
{
	std::cout << "Normal Light created!!!!!!!!!\n";
}


Light::Light(float red, float green, float blue, float ambient_intensity, float diffuse_intensity) :
	m_Colour(glm::vec3(red, green, blue)), 
	m_AmbientIntensity(ambient_intensity), m_DiffuseIntensity(diffuse_intensity),m_Disable(false)
{}

LightType Light::GetType() const
{
	return Base;
}

const char* Light::LightTypeToString()
{
	switch (m_Type)
	{
		case Point: return "Point Light";
		case Directional: return "Directional Light";
		case Spot: return "Spot Light";
	}
	return "[Error]: Type not configured or null";
}
