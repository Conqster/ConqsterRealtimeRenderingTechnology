#pragma once
#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();

	/// <summary>
	/// Configure Point Light with properites and use default attenuation
	/// </summary>
	/// <param name="red"></param>
	/// <param name="green"></param>
	/// <param name="blue"></param>
	/// <param name="ambient_intensity"></param>
	/// <param name="diffuse_intensity"></param>
	/// <param name="pos"></param>
	PointLight(float red, float green, float blue, float ambient_intensity, float diffuse_intensity, glm::vec3 pos);

	/// <summary>
	/// Configure light with all properties
	/// </summary>
	/// <param name="red"></param>
	/// <param name="green"></param>
	/// <param name="blue"></param>
	/// <param name="ambient_intensity"></param>
	/// <param name="diffuse_intensity"></param>
	/// <param name="pos"></param>
	/// <param name="cons">constant attenuation</param>
	/// <param name="linear">linear attenuation</param>
	/// <param name="quad">Quadratic Attentenuation</param>
	PointLight(float red, float green, float blue, float ambient_intensity, float diffuse_intensity, glm::vec3 pos, float cons, float linear, float quad);

	void SetAttenuation(float constant, float linear, float quadratic);
	inline float* Attenuation() { return m_AttenuationConstants; }

	void Use(int colour_location,int ambient_intensity_loc, unsigned int diffuse_intensity_loc,int position_location, int attenuationLocation);

	glm::vec3 const  GetPosition() { return m_Position; }
	inline float* const Ptr_Position() { return &m_Position[0]; }

	LightType GetType() const override;

	virtual ~PointLight();

protected:
	glm::vec3 m_Position;

	//float m_ConstantAttenuation;
	//float m_LinearAttenuation;
	//float m_QuadraticAttenuation;

	//So as to store all attenuation contiguously in memory
	float m_AttenuationConstants[3];
};