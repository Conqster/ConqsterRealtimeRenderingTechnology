#pragma once
#include "glm/glm.hpp"

enum LightType
{
	Base,
	Point, 
	Spot,
	Directional
};

class Light
{
public:
	Light();
	Light(float red, float green, float blue, float ambient_intensity, float diffuse_intensity);

	float const GetAmbientIntensity(){ return m_AmbientIntensity; }
	float const GetDiffuseIntensity() { return m_DiffuseIntensity; }
	glm::vec3 const GetColour() { return m_Colour; }

	inline float* const Ptr_AmbientIntensity() { return &m_AmbientIntensity; }
	inline float* const Ptr_DiffuseIntensity() { return &m_DiffuseIntensity; }
	inline float* const Ptr_Colour() { return &m_Colour[0]; }
	inline bool* const Ptr_Disable() { return &m_Disable; }

	virtual LightType GetType() const;

	const char* LightTypeToString();

	virtual ~Light() = default;

protected:
	glm::vec3 m_Colour;

	LightType m_Type = Base;

	float m_AmbientIntensity;
	float m_DiffuseIntensity;
	bool m_Disable;

};