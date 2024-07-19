#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(float red, float green, float blue, float ambient_intensity, float diffuse_intensity, glm::vec3 dir);

	void Use(int is_enable_loc, int colour_loc, int ambient_intensity_loc, unsigned int diffuse_intensity_loc, int direction_loc);
	inline float* const Ptr_Direction() { return &m_Direction[0]; }

	LightType GetType() const override;

	virtual ~DirectionalLight();

private:
	glm::vec3 m_Direction;
};