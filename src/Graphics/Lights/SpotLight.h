#pragma once
#include "PointLight.h"

class SpotLight : public PointLight
{
public:
	SpotLight();

	SpotLight(float red, float green, float blue, float ambient_intensity, float diffuse_intensity, glm::vec3 pos);
	SpotLight(float red, float green, float blue, float ambient_intensity, float diffuse_intensity, glm::vec3 pos, glm::vec3 dir, float falloff);
	
	void Use(int colour_location, int ambient_intensity_loc, unsigned int diffuse_intensity_loc, int position_location, int attenuationLocation, int dir_location, int falloff_location);

	glm::vec3 const  GetDirection() { return m_Direction; }
	float const GetFalloff() { return m_Falloff; }
	inline float* const Ptr_Direction() { return &m_Direction[0]; }

	inline float* const Ptr_Falloff() { return &m_Falloff; }

private:
	glm::vec3 m_Direction;
	float m_Falloff;
};