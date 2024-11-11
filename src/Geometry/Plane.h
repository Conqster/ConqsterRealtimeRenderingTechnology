#pragma once

#include "glm/glm.hpp"


//Implicit Plane
//f = [nx, ny, nz, d] => xyz normals , d => constant
class Plane
{
private:
	glm::vec4 m_NormalAndConstant;

public:
	Plane() = default;
	Plane(const glm::vec3& in_nor, float in_d = 0.0f) : m_NormalAndConstant(glm::vec4(in_nor, in_d)){}
	Plane(const glm::vec4& in_nor_d) : m_NormalAndConstant(in_nor_d){}


	//Creation 
	//static Plane CreateFromPointAndNormal(glm::vec3 point, glm::vec3 normal) { return Plane(normal, glm::dot(-normal, point)); }
	static Plane CreateFromPointAndNormal(glm::vec3 point, glm::vec3 normal) { return Plane(normal, -glm::dot(normal, point)); }


	//retrive 
	const glm::vec3& GetNormal() const { return glm::vec3(m_NormalAndConstant.x, m_NormalAndConstant.y, m_NormalAndConstant.z); }
	const glm::vec4& GetNormalAndConstant() const { return m_NormalAndConstant; }
	const float GetConstant() const { return m_NormalAndConstant.w; }


	static bool IntersectThreePlanes(const Plane& in_f1, const Plane& in_f2, const Plane& in_f3, glm::vec3& out_p)
	{
		//retrive normals 
		glm::vec3 n1 = in_f1.GetNormal();
		glm::vec3 n2 = in_f2.GetNormal();
		glm::vec3 n3 = in_f3.GetNormal();

		n1 = glm::normalize(n1);
		n2 = glm::normalize(n2);
		n3 = glm::normalize(n3);

		float c1 = in_f1.GetConstant();
		float c2 = in_f2.GetConstant();
		float c3 = in_f3.GetConstant();

		glm::vec3 n1n2 = glm::cross(n1, n2);
		float den = glm::dot(n1n2, n3);

		if (std::fabs(den) > FLT_MIN)
		{
			out_p = ((glm::cross(n3, n2) * c1 +
				glm::cross(n1, n3) * c2 -
				n1n2 * c3) / den);

			return true;
		}

		return false;
	}
};