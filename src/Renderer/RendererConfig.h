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
