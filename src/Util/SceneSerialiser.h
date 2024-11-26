#pragma once

#include "Renderer/Lights/Lights.h"
#include "YAMLHelperSerialiser.h"


struct SceneCameraData
{
	glm::vec3 m_Position = glm::vec3(0.0f, 7.0f, -36.0f);
	glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
	float m_Yaw = 90.0f;
	float m_Pitch = 0.0f;
	float m_MoveSpeed = 20.0f;
	float m_RotSpeed = 1.0f;
};

struct SceneLightData
{
	DirectionalLight dir_Light;
	PointLight *ptLights;
	unsigned m_PtLightCount = 0;
};


struct SceneShadowData
{
	//directional
	unsigned int m_DirDepthResQuality = 2048;
	glm::vec3 m_DirSamplePos = glm::vec3(0.0f);
	float m_DirZFar = 300.0f;
	float m_DirZNear = 0.1f;
	float m_DirSampleOffset = 64.0f;
	float m_DirOrthoSize = 95.0f;

	//point
	unsigned int m_PtDepthResQuality = 1024;
	float m_PtZFar = 0.1f;
	float m_PtZNear = 0.1f;

	//utilises
	std::string m_depthVerShader;
	std::string m_depthFragShader;
	std::string m_depthGeoShader;
};

struct SceneData
{
	std::string m_Name;
	bool m_HasLight;
	SceneCameraData m_Camera;
	SceneLightData m_Light;
	SceneShadowData m_Shadow;
};

class SceneSerialiser
{
public:
	SceneSerialiser(const SceneSerialiser&) = delete;

	static inline SceneSerialiser& Instance()
	{
		static SceneSerialiser instance;
		return instance;
	}

	bool LoadScene(const std::string& file, SceneData& scene_data);
	bool RetrieveLights(const YAML::Node data, SceneLightData& scene_light);
	bool RetrieveCameraData(const YAML::Node data, SceneCameraData& scene_camera);
	bool RetrieveShadowSetting(const YAML::Node data, SceneShadowData& shadow_data);

	void SerialiseScene(const std::string& file, SceneData scene_data);


	void SerialiseShader(const std::string& file, class Shader shader);
private: 
	SceneSerialiser() = default;


	//Serialisers
	void SerialiseLightData(YAML::Emitter& out, SceneLightData& light_data);
	void SerialiseCameraData(YAML::Emitter& out, SceneCameraData& camera_data);
	void SerialiseShadowSetting(YAML::Emitter& out, SceneShadowData& shadow_data);
};

