#pragma once

#include "Plane.h"
#include <glm/gtc/matrix_transform.hpp>


class Frustum
{
public:
	Frustum() = default;

	Frustum(const glm::vec3& pos, const glm::vec3& forward, glm::vec3 _up, const float cam_near, const float cam_far, float fov, float aspect_ratio)
	{
		//Near plane
		m_Planes[0] = Plane::CreateFromPointAndNormal(pos + (cam_near * forward), forward);
		//far 
		m_Planes[1] = Plane::CreateFromPointAndNormal(pos + (cam_far * forward), -forward);

		//cam half fov_y & half fov_x 
		float h_fov_y = glm::radians(fov) * 0.5f;
		float h_fov_x = glm::atan(glm::tan(h_fov_y) * aspect_ratio);

		glm::vec3 right = glm::cross(forward, _up);
		right = glm::normalize(right);
		glm::vec3 up = glm::cross(right, forward);
		up = glm::normalize(up);

		//left & right normals 
		glm::vec3 lt_plane_nor = glm::rotate(glm::mat4(1.0f), -h_fov_x, up) * glm::vec4(-right, 0.0f);
		glm::vec3 rt_plane_nor = glm::rotate(glm::mat4(1.0f), h_fov_x, up) * glm::vec4(right, 0.0f);
		//left & right planes
		m_Planes[2] = Plane::CreateFromPointAndNormal(pos, lt_plane_nor);
		m_Planes[3] = Plane::CreateFromPointAndNormal(pos, rt_plane_nor);


		//top & bottom normals 
		glm::vec3 tp_plane_nor = glm::rotate(glm::mat4(1.0f), h_fov_y, right) * glm::vec4(-up, 0.0f);
		glm::vec3 bm_plane_nor = glm::rotate(glm::mat4(1.0f), -h_fov_y, right) * glm::vec4(up, 0.0f);
		//top & bottom planes
		m_Planes[4] = Plane::CreateFromPointAndNormal(pos, tp_plane_nor);
		m_Planes[5] = Plane::CreateFromPointAndNormal(pos, bm_plane_nor);
	}


	static enum class Planes_side : uint8_t
	{
		Near = 0,
		Far = 1,
		Left = 2,
		Right = 3,
		Top = 4,
		Bottom = 5,
	};

	const Plane* GetPlanes() const { return m_Planes; }
	const Plane& GetPlane(Planes_side side) const { return m_Planes[(int)side]; }
	const Plane& GetPlane(int side) const { return m_Planes[side]; }


	/// <summary>
	/// Current issue if all points are out of view for a hugh AABB,
	/// but one or more of its edge is in view there is no calculation to confirm its visisblity 
	/// </summary>
	/// <param name="aabb"></param>
	/// <returns></returns>
	inline bool PointsInFrustum(const AABB& aabb) const
	{
		//check if center is in frustum 
		if (!InFrustum(aabb.GetCenter()))
		{
			//if center is not,
			//check for other possible aabb points 
			//return the first successful 
			glm::vec3 min = aabb.m_Min;
			glm::vec3 max = aabb.m_Max;
			bool in_frustum = InFrustum(min);
			if (in_frustum) return true;
			in_frustum = InFrustum(max);
			if (in_frustum) return true;
			in_frustum = InFrustum(glm::vec3(max.x, min.y, min.z));
			if (in_frustum) return true;
			in_frustum = InFrustum(glm::vec3(max.x, max.y, min.z));
			if (in_frustum) return true;
			in_frustum = InFrustum(glm::vec3(min.x, max.y, min.z));
			if (in_frustum) return true;
			in_frustum = InFrustum(glm::vec3(min.x, max.y, max.z));
			if (in_frustum) return true;
			in_frustum = InFrustum(glm::vec3(max.x, min.y, max.z));
			if (in_frustum) return true;
			
			//return last if failed/passed 
			return InFrustum(glm::vec3(min.x, min.y, max.z));
		}
		return true;
	}


	/// <summary>
	/// 
	/// </summary>
	/// <param name="aabb"></param>
	/// <returns></returns>
	inline bool IsAABBVisible(const AABB& aabb) const
	{
		for (unsigned int i = 0; i < 6; i++)
		{
			glm::vec3 nor = m_Planes[i].GetNormal();
			nor = glm::normalize(nor);
			float d = m_Planes[i].GetConstant();
			float rg = std::fabs(nor.x * aabb.GetHalfSize().x) + 
					   std::fabs(nor.y * aabb.GetHalfSize().y) + 
					   std::fabs(nor.z * aabb.GetHalfSize().z);
			if (glm::dot(nor, aabb.GetCenter()) <= -rg - d) return false;
		}
		return true;
	}

	inline bool InFrustum(const glm::vec3& point) const
	{
		for (unsigned int i = 0; i < 6; i++)
		{
			glm::vec3 nor = m_Planes[i].GetNormal();
			nor = glm::normalize(nor);
			float d = m_Planes[i].GetConstant();

			if ((glm::dot(nor, point) + d) < 0)
				return false;
		}
		return true;
	}


private:
	Plane m_Planes[6];
};