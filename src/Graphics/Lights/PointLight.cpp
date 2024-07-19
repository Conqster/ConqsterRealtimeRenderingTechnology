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

void PointLight::Use(int enable_loc, int colour_loc,int ambient_intensity_loc, unsigned int diffuse_intensity_loc,int position_loc, int attenuation_loc)
{
	//TO-DO: just hack for now
	GLCall(glUniform1i(enable_loc, m_Disable));


	GLCall(glUniform3f(colour_loc, m_Colour.x, m_Colour.y, m_Colour.z));
	GLCall(glUniform1f(ambient_intensity_loc, m_AmbientIntensity));
	GLCall(glUniform1f(diffuse_intensity_loc, m_DiffuseIntensity));
	GLCall(glUniform3f(position_loc, m_Position.x, m_Position.y, m_Position.z));

	GLCall(glUniform3fv(attenuation_loc, 1, m_AttenuationConstants));
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
