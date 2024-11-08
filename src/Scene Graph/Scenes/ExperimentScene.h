#pragma once
#include "Scene.h"

#include <vector>
#include <memory>

#include "Util/ModelLoader.h"
#include "Renderer/ObjectBuffer/UniformBuffer.h"

#include "Renderer/RendererConfig.h" //update when DirLightObject is update
#include "Renderer/Lights/Lights.h"
#include "Renderer/ObjectBuffer/ShadowMap.h"

#include "Renderer/Shader.h"

#include "Renderer/Skybox.h"  //update when Skybox is update
#include "Util/ModelLoaderN.h"   //new model loader 


class Entity;
struct Material;
class Shader;
class Framebuffer;
class UniformBuffer;

class ExperimentScene : public Scene
{
public:
	ExperimentScene() = default;


	virtual void SetWindow(Window* window);
	inline Camera* const GetCamera() const { return m_Camera; }

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

private:

	//Scene Construction
	void InitRenderer();
	void CreateEntities();  // Or Load from file
	void CreateGPUDatas();
	void CreateLightDatas(); //might need to move this 


	//Scene Begin Render
	void BeginRenderScene();
	void PreUpdateGPUUniformBuffers(); //camera ubo
	void BuildSceneEntities();
	void ShadowPass(Shader& depth_shader);

	//Scene Render
	void PostUpdateGPUUniformBuffers(); //light ubo after scene is sorted & light pass
	void DrawScene();

	//Scene Post Render
	//No after Effect at the moment 

	//After Render 
	void SceneDebugger();
	void DebugEntitiesPos(Entity& enitity);
	//void AfterRender(); //<------------- For clean ups that is required after rendering 

	//scene special data
	bool enableSceneShadow = true;


	//Resource
	std::vector<std::shared_ptr<Entity>> m_SceneEntities;
	Shader m_SceneShader;//std::vector<std::shared_ptr<Shader>> m_SceneShaders;
	Shader shadowDepthShader;  //this is not scene deoth shader
	std::vector<std::shared_ptr<Material>> m_SceneMaterials;
	Skybox m_Skybox;
	////Later have a structure for RenderData
	//struct MeshRenderer
	//{
	//	std::shared_ptr<Mesh> mesh;
	//	glm::mat4 world_pos; 
	//	//std::shared_ptr<Material> mat;
	//	////extra
	//	//bool cast_shadow;
	//	//bool receive_shadow;
	//	////Or maybe a layer based
	//};

	//std::vector<std::shared_ptr<Framebuffer>> m_SceneFBOs; //Lateeeerrr

	//Utilities
	ModelLoader m_ModelLoader;
	CRRT::ModelLoader m_NewModelLoader;

	std::shared_ptr<Model> modelWcNewLoader;
	glm::mat4 modelWcNewLoaderTrans = glm::mat4(1.0f);

	//UniformBuffers
	UniformBuffer m_CamMatUBO;
	UniformBuffer m_LightDataUBO;
	UniformBuffer m_EnviUBO;


	//Dealing with models
	std::shared_ptr<Model> blenderShapes;
	glm::mat4 shapesTrans = glm::mat4(1.0f);


	////////////////////////////////
	// HAVE TO REMOVE THIS LATER 
	////////////////////////////////
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
	//Skybox enviromwent
	bool m_UseSkybox = false;
	float m_SkyboxInfluencity = 0.2f;
	float m_SkyboxReflectivity = 0.8f;


	//------------------------------Utility functions------------------------/
	//Quick Hack
	void RenderEnitiyMesh(Shader& shader, const std::shared_ptr<Entity>& entity, bool use_mat = false);


	void MaterialShaderBindHelper(Material& mat, Shader& shader);
	void MainUI();
	void EnititiesUI();
	void EntityDebugUI(Entity& entity);
	void MaterialsUI();
	//void FrameBufferUI();


	void EntityModelMaterial(const Entity& entity);
};