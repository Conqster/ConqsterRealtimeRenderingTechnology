#pragma once

#include "glm/glm.hpp"

class AABB
{
public:
	glm::vec3 m_Min = glm::vec3();
	glm::vec3 m_Max = glm::vec3();

	//Constructor
	AABB() : m_Min(glm::vec3(-1.0f, 1.0f, 1.0f)), m_Max(glm::vec3(1.0f, -1.0f, -1.0f)){}
	AABB(float size) : m_Min(glm::vec3(-1.0f, 1.0f, 1.0f) * size), m_Max(glm::vec3(1.0f, -1.0f, -1.0f) * size){}
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
		m_Min = (m_Min + inTranslation);
		m_Max = (m_Max + inTranslation);
	}
};