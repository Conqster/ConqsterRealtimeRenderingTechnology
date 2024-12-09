#pragma once

#include "Renderer/Lights/Lights.h"
#include "YAMLHelperSerialiser.h"


struct SceneCameraData
{
	glm::vec3 m_Position = glm::vec3(0.0f, 7.0f, -36.0f);
	float m_Yaw = 90.0f;
	float m_Pitch = 0.0f;
	float m_Fov = 45.0f;
	float m_Near = 0.1f;
	float m_Far = 150.0f;
	float m_MoveSpeed = 20.0f;
	float m_RotSpeed = 1.0f;
};

struct SceneLightData
{
	DirectionalLight dir_Light;
	//PointLight *ptLights;
	std::vector<PointLight> ptLights;
	unsigned m_PtLightCount = 0;
};


struct ShaderData
{
	std::string m_Name = "default/not_set";
	std::string m_Vertex = "";
	std::string m_Fragment = "";
	std::string m_Geometry = "";
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

struct SceneRenderingConfig
{
	ShaderData m_ForwardShader;
	//ResolutionSetting m_ResQuality;
	//have render options: Forward shadering/deffered shading
	ShaderData m_GBufferShader;
	ShaderData m_DeferredShader;
};

struct SkyboxTextures
{
	std::string tex_rt;
	std::string tex_lt;
	std::string tex_tp;
	std::string tex_bm;
	std::string tex_ft;
	std::string tex_bk;
};
struct SceneEnvironment
{
	bool m_EnableSkybox = true;
	float m_SkyboxInfluencity = 0.2f;
	float m_SkyboxReflectivity = 0.8f;
	SkyboxTextures m_SkyboxTex;
	ShaderData m_SkyboxShader;
};


struct SceneData
{
	std::string m_Name;
	bool m_HasLight;
	bool m_HasShadow = true;
	bool m_HasEnvironmentMap = true;
	SceneCameraData m_Camera;
	SceneRenderingConfig m_RenderingConfig;
	SceneEnvironment m_SceneEnvi;
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
	bool RetrieveRenderConfig(const YAML::Node data, SceneRenderingConfig& render_config);
	bool RetrieveEnvironmentData(const YAML::Node data, SceneEnvironment& envir_data);
	bool RetrieveCameraData(const YAML::Node data, SceneCameraData& scene_camera);
	bool RetrieveShadowSetting(const YAML::Node data, SceneShadowData& shadow_data);

	bool RetrieveShader(const YAML::Node data, ShaderData& shader_data);

	void SerialiseScene(const std::string& file, SceneData scene_data);


	//void SerialiseShader(const std::string& file, class Shader shader);
private: 
	SceneSerialiser() = default;


	//Serialisers
	void SerialiseLightData(YAML::Emitter& out, SceneLightData& light_data);
	void SerialiseRenderingConfig(YAML::Emitter& out, SceneRenderingConfig& rendering_config);
	void SerialiseEnvironmentData(YAML::Emitter& out, SceneEnvironment& envir_data);
	void SerialiseCameraData(YAML::Emitter& out, SceneCameraData& camera_data);
	void SerialiseShadowSetting(YAML::Emitter& out, SceneShadowData& shadow_data);


	//Helper
	void SerialiseShader(YAML::Emitter& out, ShaderData shader_data);
};

