#include "PointLight.h"
#include <iostream>

#include "GL/glew.h"
#include "../Renderer.h"

PointLight::PointLight() : Light(), m_Position(glm::vec3())
{
	SetAttenuation(0.12f, 0.07f, 0.05f);
	m_Type = Point;
	std::cout << "Point Light created!!!!!!!!\n";
}

PointLight::PointLight(float red, float green, float blue, float ambient_intensity, float diffuse_intensity, glm::vec3 pos) :
				Light(red, green, blue, ambient_intensity, diffuse_intensity), m_Position(pos)
{
	m_Type = Point;
	SetAttenuation(0.12f, 0.07f, 0.05f);
	std::cout << "Point Light created!!!!!!!!\n";
}

PointLight::PointLight(float red, float green, float blue, float ambient_intensity, float diffuse_intensity, 
						glm::vec3 pos, float cons, float linear, float quad) :
				Light(red, green, blue, ambient_intensity, diffuse_intensity), m_Position(pos)
{
	m_Type = Point;
	SetAttenuation(cons, linear, quad);
	std::cout << "Point Light created!!!!!!!!\n";
}

void PointLight::SetAttenuation(float constant, float linear, float quadratic)
{
	m_AttenuationConstants[0] = constant;
	m_AttenuationConstants[1] = linear;
	m_AttenuationConstants[2] = quadratic;
}

void PointLight::Use(int colour_location,int ambient_intensity_loc, unsigned int diffuse_intensity_loc,int position_location, int attenuationLocation)
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
	//GLCall(glUniform1f(attenuationLocation, m_AttenuationConstants));

}

LightType PointLight::GetType() const
{
	return Point;
}

PointLight::~PointLight()
{
	std::cout << "Point Light destroied\n";
}

