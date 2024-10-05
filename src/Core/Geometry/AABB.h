#pragma once
#include "glm/glm.hpp"

class AABB
{
public:
	glm::vec3 m_Min = glm::vec3(0.0f);
	glm::vec3 m_Max = glm::vec3(0.0f);

	//Constructor
	AABB() : m_Min(glm::vec3(-0.5f)), m_Max(glm::vec3(0.5f)){}
	AABB(float inSize) : m_Min(glm::vec3(-1.0f) * inSize), m_Max(glm::vec3(1.0f) * inSize){}
	AABB(glm::vec3 inPoint) : m_Min(inPoint), m_Max(inPoint){} //used if only a point is defines
	AABB(glm::vec3 inMin, glm::vec3 inMax) : m_Min(inMin), m_Max(inMax){}

	glm::vec3 GetCenter() const
	{
		return (m_Min + m_Max) * 0.5f;
	}

	glm::vec3 GetSize() const
	{
		return m_Max - m_Min;
	}

	void Translate(glm::vec3 inTranslation)
	{
		m_Min += inTranslation;
		m_Max += inTranslation;
	}

	void Scale(glm::vec3 inScale)
	{
		m_Min -= inScale;
		m_Max += inScale;
	}

	void Encapsulate(glm::vec3 inPos)
	{
		m_Min = (glm::min)(m_Min, inPos);
		m_Max = (glm::max)(m_Max, inPos);
	}

	void Encapsulate(const AABB &inRHS)
	{
		m_Min = (glm::min)(m_Min, inRHS.m_Min);
		m_Max = (glm::max)(m_Max, inRHS.m_Max);
	}

	std::vector<glm::vec3> GetVertices()
	{

	}
};