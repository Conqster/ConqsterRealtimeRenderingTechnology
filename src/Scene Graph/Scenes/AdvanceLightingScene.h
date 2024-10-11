#pragma once
#include "Scene.h"
#include "Scene Graph/Model.h"

#include "Renderer/Renderer.h"
#include "Renderer/Meshes/Meshes.h"
#include "Renderer/Lights/Lights.h"
#include "Renderer/ObjectBuffer/ShadowMap.h"
#include "Renderer/ObjectBuffer/UniformBuffer.h"

#include "Geometry/AABB.h"
#include "Util/Constants.h"
#include "Util/ModelLoader.h"


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

	void UpdateViewMatrix(glm::vec3 sample_pos, glm::vec3 dir,float offset = 1.0f)
	{
		view = glm::lookAt(sample_pos + (dir * offset),
			sample_pos, glm::vec3(0.0f, 1.0f, 0.0f)); //world up 0, 1, 0
	}

};


class AdvanceLightingScene : public Scene
{
public:
	AdvanceLightingScene() = default;

	virtual void SetWindow(Window* window);
	inline Camera* const GetCamera() const { return m_Camera; }

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;
private:
	void CreateObjects();
	void DrawObjects(Shader& shader);
	void LightPass(Shader& shader);
	void InstanceObjectPass(Shader* shader = nullptr);
	void ShadowPass();


	////////////////
	// Utilities 
	////////////////
	ModelLoader modelLoader;
	UniformBuffer m_CameraMatUBO;
	bool doGammaCorrection = false;
	float gamma = 2.2f;
	LearnVertex m_Quad;
	UniformBuffer m_LightDataUBO;

	/////////////////
	//Debugging datas
	/////////////////
	float normDebugLength = 0.0f; // 0.03f;
	glm::vec3 normDebugColour = glm::vec3(1.0f, 0.0f, 1.0f);
	bool useDebugColour = true;
	bool debugLightPos = true;
	bool debugScene = false;
	bool debugVertexPosColour = false;
	bool disableTexture = true;
	bool useBlinnPhong = true;
	enum DebugModelType : int
	{
		MODEL_SPACE,
		NORMAL,
		MODEL_NORMAL,
		MODEL_ABS_NORMAL,
		MODEL_COLOUR,
		DEFAULT_COLOUR
	}debugModelType;

	//////////////
	// Objects
	//////////////
	//sphere
	SphereMesh sphere;
	//glm::vec3 spherePos = glm::vec3(0.0f);
	//float sphereScale = 1.0f;
#define MAX_SPHERE 5
	glm::vec3 spheresPos[MAX_SPHERE];
	float spheresScale[MAX_SPHERE];
	bool debugSphereAABB = false;
	bool debugAllSphereAABB = false;
#define MAX_SPHERE_INSTANCE 100
	std::vector<glm::vec3> sphereInstancePos = std::vector<glm::vec3>();
#define MAX_CUBE 5 //current max cube = 5
	glm::vec3 cubesPos[MAX_CUBE]; 
	float cubesScale[MAX_CUBE]; 
	bool debugCubeAABB = false;
	//ground
	SquareMesh ground;
	glm::vec3 groundPos = glm::vec3();
	float groundScale = 50.0f;
	//cube 
	CubeMesh cube;
	//model1 (stanford-bunny)
	std::shared_ptr<Model> model_1;
	//glm::vec3 model_1Pos = glm::vec3(0.0f);
	//float model_1Scale = 20.0f;
#define MAX_BUNNY_MODEL 5
	glm::vec3 bunnysPos[MAX_BUNNY_MODEL];
	float bunnysScale[MAX_BUNNY_MODEL];
	bool debugBunnyAABB = false;
	bool reCalBunnyAABB = false;
	glm::vec3 centerOffset;
	float scaleBunnyBy = 1.0f;
	//model2 (unknown) 
	std::shared_ptr<Model> model_2;
	glm::vec3 model_2Pos = glm::vec3(20.0f, 1.5f, 10.0f);
	float model_2Scale = 1.0f;
	bool debugModel2AABB = false;
	glm::vec3 model2AABBCenterOffset;
	float model2AABBScale = 1.0f;

	///////////////
	// Shaders
	///////////////
	Shader modelShader;
	Shader debugShader;
	Shader instancingShader;
	Shader screenShader;
	Shader debugPtLightMapShader;

	bool useNewShader = true;
	//////Textures
	Texture* brickTex;
	Texture* plainTex;
	Texture* manchesterTex;


	///////////////////////
	// Lights
	///////////////////////
	int availablePtLightCount = 0;
	int specShinness = 64;
	//--------------------General Shadow Data----------------------------------------/
	Shader shadowDepthShader;


	//---------------------Directional Light data------------------------------------------/
	struct DirLightObject
	{
		glm::vec3 sampleWorldPos = glm::vec3(0.0f);
		float cam_offset = 5.0f;
		DirectionalLight dirlight;  //Directional light
		DirShadowCalculation dirLightShadow; //direction light shadow data
	}dirLightObject;
	ShadowMap dirDepthMap;


	//---------------------Point Light data------------------------------------------/
	const static int MAX_LIGHT = 5;
	struct PointLightObject
	{
		glm::vec3 objectPosition;
		float childLightOffset = 0.0f;
		float moveSpeed = 0.0f;
		PointLight light;
	}lightObject[MAX_LIGHT];
	ShadowConfig ptShadowConfig;
	std::vector<ShadowCube> ptDepthMapCubes;


	//---------------------------------------------Helper----------------------------------------
	std::vector<glm::mat4> PointLightSpaceMatrix(glm::vec3 pos, ShadowConfig config = {ResolutionSetting::LOW_RESOLUTION, 0.1f, 25.0f})
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


	struct PlayerTest
	{
		AABB aabb = AABB(2.0f);
		float speed = 0.2f;
		float debugThick = 1.0f;
		float shadowOffset = 10.0f;
	}playerTest;



	//ShadowResolutionSettings Enum
	//Low = 1024        
	//Medium = 2048
	//Hight = 4096
	// Point Light quality is half of Dir
	// i.e Low for Point Light == 1024 * 0.5 
	//Later change shadowDataToconfiguratiomn like near, far
	// struct 
	// -near
	// -far
	// - dir size
	// - ShadowResolutionSettings Enum

};
