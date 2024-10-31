#pragma once
#include "Scene.h"

#include "Renderer/Renderer.h"
#include "Renderer/Meshes/Meshes.h"
#include "Renderer/Lights/Lights.h"
#include "Renderer/ObjectBuffer/ShadowMap.h"

#include "Renderer/Material.h"


enum ResolutionSetting
{
	LOW_RESOLUTION,
	MEDUIM_RESOLUTION,
	HIGH_RESOLUTION
};

struct ShadowConfig
{
	ResolutionSetting res = LOW_RESOLUTION;

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



class ParallaxExperimentalScene : public Scene
{
public:
	ParallaxExperimentalScene() = default;


	virtual void SetWindow(Window* window);
	inline Camera* const GetCamera() const { return m_Camera; }

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;
private:
	void CreateObjects();
	void DrawObjects(Shader& shader, bool nor_map = false);
	void LightPass();
	void ShadowPass();


	///////////////
	// Shaders
	///////////////
	Shader modelShader;
	Shader shadowDepthShader;

	//////Textures
	//Texture* brickTex;
	//Texture* brickNorMap;

	//Test material 
	Material floorMat;
	Material planeMat;

	//ground
	SquareMesh ground;
	glm::vec3 groundPos = glm::vec3();
	float groundScale = 50.0f;
	bool useNor = true;

	//Plane 
	//plane uses the same mesh with ground 
	glm::mat4 planeWorldTran = glm::mat4(1.0f);

	/////////////////////////
	// UNIFROM BUFFERS
	/////////////////////////
	UniformBuffer m_CamMatUBO;
	UniformBuffer m_LightDataUBO;

	//---------------------Introduce material system------------------------------/


	//---------------------------Lighting Utilities--------------------------/
	//Directional Light data-------------------/
	struct DirLightObject
	{
		glm::vec3 sampleWorldPos = glm::vec3(0.0f);
		float cam_offset = 5.0f;
		DirectionalLight dirlight;  //Directional light
		DirShadowCalculation dirLightShadow; //direction light shadow data
	}dirLightObject;
	ShadowMap dirDepthMap;
	//Point Light data-------------------/
	PointLight ptLight;
	ShadowConfig ptShadowConfig;
	ShadowCube ptDepthCube;


	//---------------------------------------------Helper----------------------------------------
	std::vector<glm::mat4> PointLightSpaceMatrix(glm::vec3 pos, ShadowConfig config = { ResolutionSetting::LOW_RESOLUTION, 0.1f, 25.0f })
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