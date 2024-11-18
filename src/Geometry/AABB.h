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

	glm::vec3 const GetHalfSize() const
	{
		return (m_Max - m_Min) * 0.5f;
	}

	void Translate(glm::vec3 inTranslation)
	{
		m_Min += inTranslation;
		m_Max += inTranslation;
	}


	/// <summary>
	/// To work with meshes use as 
	/// temp.Scale((scale - glm::vec3(1.0f)) * glm::vec3(0.5f));
	/// as normal scale would be glm::vec3(1.0f) and using scale 
	/// as raw would scale AABB by 1 unnessary unit & the half glm::vec3(0.5f) 
	/// is to compensate that AABB is scaled in both min and max
	/// </summary>
	/// <param name="inScale"></param>
	void Scale(glm::vec3 inScale)
	{
		m_Min -= inScale;
		m_Max += inScale;
	}

	AABB Tranformed(glm::mat4 in_Transform)
	{
		//New AABB center to be in tranform pos
		glm::vec3 temp_min = in_Transform[3], 
				  temp_max = in_Transform[3];


		for (unsigned int i = 0; i < 3; i++)
		{
			glm::vec3 column = in_Transform[i];

			glm::vec3 a = column * m_Min[i];
			glm::vec3 b = column * m_Max[i];

			temp_min += (glm::min)(a, b);
			temp_max += (glm::max)(a, b);
		}
		
		return AABB(temp_min, temp_max);
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

};