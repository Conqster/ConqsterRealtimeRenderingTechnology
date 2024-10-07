#pragma once
#include "Scene.h"

#include "SceneGraph/ModelLoader.h"
#include "SceneGraph/Model.h"

#include "Graphics/Shader.h"
#include "Graphics/ObjectBuffer/UniformBuffer.h"

#include "Graphics/Meshes/SphereMesh.h"
#include "Graphics/Meshes/SquareMesh.h"
#include "Graphics/Meshes/CubeMesh.h"

#include "Graphics/Lights/NewLights.h"
#include "Util/Constants.h"

#include "Graphics/ObjectBuffer/ShadowMap.h"

#include "Geometry/AABB.h"


struct ShadowDataInfo
{
	///////
	//DIRECTIONAL LIGHT
	///////
	//Projection => ortho cam / use glm
	//glm::orth(left, right, bottom, top, near, far)
	//min bounds => left = bottom => -cam_size
	//max bounds => right = top => cam_size
	//
	//View => lookAt / use glm
	//glm::lookAt(cam_offset, world_space_focus, world_up)
	//world space focus => for naive optimisation the center for cam to focus relative (sample_pos)
	//					   to user/player view, i.e we would have to use world origin.
	//cam offset => offset camera from world_space_focus(sample_pos) based on light direction & and offset rate
	// 
	// lightSpaceMatrix => Projection * View
	//
	///////
	// POINT LIGHT 
	///////
	//Projection => persp cam /glm
	//all light shares proj for now 
	//but to calculate the light mtx by is pos. 


	//////////////////
	// Utilies
	//////////////////
	//Shadow map => a texture map to store world/sample view depth from light perspective
	//DepthTest&Debug shader => for depth calculation from lights perspective 



	//////////////////
	//Properties
	//////////////////
	float cam_near = 0.2f;
	float cam_far = 75.0f;
	//Directional use size instead fov
	float cam_size = 40.0f;

	//Directional world sample/focus position
	glm::vec3 sampleWorldPos = glm::vec3(0.0f);
	float cam_offset = 5.0f;

	//Utilities
	ShadowMap depthMap;
	ShadowCube depthCube;
	Shader depthShader;
	bool debugPara = true;  //debug pos & parameters
	int debugCubeFaceIdx = 0;

	//Cache Matrix
	glm::mat4 proj; //dir proj
	glm::mat4 view;

	glm::mat4 GetLightSpaceMatrix()
	{
		return proj * view;
	}

	void UpdateProjMat()
	{
		proj = glm::ortho(-cam_size, cam_size,
			-cam_size, cam_size,
			cam_near, cam_far);
	}

	void UpdateViewMatrix(glm::vec3 dir)
	{
		view = glm::lookAt(sampleWorldPos + (dir * cam_offset),
			sampleWorldPos, glm::vec3(0.0f, 1.0f, 0.0f)); //world up 0, 1, 0
	}



	/////////////////////////////////////////////
	// Point Light
	/////////////////////////////////////////////

	std::vector<glm::mat4> PointLightSpaceMatrix(glm::vec3 pos)
	{
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

	void UpdatePointLightProjMat()
	{
		//float aspect = 1.0f; //because width == height 
		proj = glm::perspective(glm::radians(90.0f), 1.0f, cam_near, cam_far);
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

	/////////////////
	//Debugging datas
	/////////////////
	float normDebugLength = 0.0f; // 0.03f;
	glm::vec3 normDebugColour = glm::vec3(1.0f, 0.0f, 1.0f);
	bool useDebugColour = true;
	bool debugLightPos = true;
	bool debugScene = false;
	//bool debugWcModelSpace = false;
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

	//////Textures
	Texture* brickTex;
	Texture* plainTex;
	Texture* manchesterTex;


	///////////////////////
	// Lights
	///////////////////////
	//NewPointLight pointLights[Shader_Constants::MAX_POINT_LIGHTS];
	const static int MAX_LIGHT = 5;
	struct PointLightObject
	{
		glm::vec3 objectPosition;
		float childLightOffset = 0.0f;
		float moveSpeed = 0.0f;
		NewPointLight light;
		ShadowDataInfo shadowData;
		//ShadowCube depthCube;
	}lightObject[MAX_LIGHT];
	//NewPointLight pointLights[MAX_LIGHT];
	//glm::vec3 pointLocalWorldPosition[MAX_LIGHT]; //probably change later to local relative to parent
	int availablePtLightCount = 0;
	int specShinness = 64;

	
	NewDirectionalLight dirlight;  //Directional light
	ShadowDataInfo dirLightShadow; //direction light shadow data
	//ShadowDataInfo ptLightShadow;


	struct ShadowConfig
	{
		float cam_near = 0.1f;
		float cam_far = 25.0f;



		//Debuging Para
		bool debugLight;
		int debugLightIdx = 0;
		int debugCubeFaceIdx = 0;
	}ptShadowConfig;
	//ShadowCube depthCube;]
	std::vector<ShadowCube> ptDepthMapCubes;
	Shader depthShader;
	//---------------------------------------------Helper----------------------------------------
	std::vector<glm::mat4> PointLightSpaceMatrix(glm::vec3 pos, ShadowConfig config = {0.1f, 25.0f})
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

	enum ShadowSamplingType : int
	{
		SHADOW_PCF_SAMPLING,
		SHADOW_POISSON_SAMPLING,
	}shadowSamplingType;



	//ShadowResolutionSettings Enum
	//Low = 1024
	//Medium = 2048
	//Hight = 4096
	//Later change shadowDataToconfiguratiomn like near, far
	// struct 
	// -near
	// -far
	// - dir size
	// - ShadowResolutionSettings Enum

};
