#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"  //remove when Shadow Calculation is removed

enum class ResolutionSetting
{
	LOW_RESOLUTION,
	MEDUIM_RESOLUTION,
	HIGH_RESOLUTION
};

struct ShadowConfig
{
	ResolutionSetting res = ResolutionSetting::LOW_RESOLUTION;

	float cam_near = 0.1f;
	float cam_far = 25.0f;

	float cam_size = 40.0f;//directional light

	//Debuging Para
	bool debugLight = false;
	int debugLightIdx = 0;
	int debugCubeFaceIdx = 0;
};



struct DirShadowCalculation
{
	ShadowConfig config;

	bool debugPara = true;  //debug pos & parameters
	float cam_offset = 5.0f;
	glm::vec3 sampleWorldPos = glm::vec3(0.0f);

	//Cache Matrix
	glm::mat4 proj;
	glm::mat4 view;

	glm::mat4 GetLightSpaceMatrix()
	{
		return proj * view;
	}

	//-----------------Variables "proj & view" might not change over multiple frames--------/
	void UpdateProjMat()
	{
		proj = glm::ortho(-config.cam_size, config.cam_size,
			-config.cam_size, config.cam_size,
			config.cam_near, config.cam_far);
	}

	void UpdateViewMatrix(glm::vec3 sample_pos, glm::vec3 dir, float offset = 1.0f)
	{
		view = glm::lookAt(sample_pos + (dir * offset),
			sample_pos, glm::vec3(0.0f, 1.0f, 0.0f)); //world up 0, 1, 0
	}

};


struct PointShadowCalculation
{
	static std::vector<glm::mat4> PointLightSpaceMatrix(glm::vec3 pos, ShadowConfig config = { ResolutionSetting::LOW_RESOLUTION, 0.1f, 25.0f })
	{
		glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, config.cam_near, config.cam_far);
		glm::mat4 view;
		std::vector<glm::mat4> tempMatrix;

		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);


		//Light right view
		view = glm::lookAt(pos, pos + right, -up);
		tempMatrix.push_back(proj * view);

		//Light left view
		view = glm::lookAt(pos, pos - right, -up);
		tempMatrix.push_back(proj * view);

		//Light top view
		view = glm::lookAt(pos, pos + up, forward);
		tempMatrix.push_back(proj * view);

		//Light bottom view
		view = glm::lookAt(pos, pos - up, -forward);
		tempMatrix.push_back(proj * view);

		//Light near/back view
		view = glm::lookAt(pos, pos + forward, -up);
		tempMatrix.push_back(proj * view);

		//Light far/forward view
		view = glm::lookAt(pos, pos - forward, -up);
		tempMatrix.push_back(proj * view);

		return tempMatrix;
	}
};
