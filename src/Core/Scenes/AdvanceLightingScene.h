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


	////////////////
	// Utilities 
	////////////////
	ModelLoader modelLoader;
	UniformBuffer m_CameraMatUBO;
	bool doGammaCorrection = false;
	float gamma = 2.2f;

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
#define MAX_SPHERE_INSTANCE 100
	std::vector<glm::vec3> sphereInstancePos = std::vector<glm::vec3>();
#define MAX_CUBE 5 //current max cube = 5
	glm::vec3 cubesPos[MAX_CUBE]; 
	float cubesScale[MAX_CUBE]; 
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
	//model2 (unknown) 
	std::shared_ptr<Model> model_2;
	glm::vec3 model_2Pos = glm::vec3(20.0f, 1.5f, 10.0f);
	float model_2Scale = 1.0f;

	///////////////
	// Shaders
	///////////////
	Shader modelShader;
	Shader debugShader;
	Shader posDebugShader;
	Shader instancingShader;

	//////Textures
	Texture* brickTex;
	Texture* plainTex;
	Texture* manchesterTex;


	///////////////////////
	// Lights
	///////////////////////
	//NewPointLight pointLights[Shader_Constants::MAX_POINT_LIGHTS];
	const static int MAX_LIGHT = 5;
	NewPointLight pointLights[MAX_LIGHT];
	int availablePtLightCount = 0;
	int specShinness = 64;

};
