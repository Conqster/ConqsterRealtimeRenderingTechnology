#pragma once
#include "Scene.h"

#include <vector>
#include <memory>

#include "Renderer/ObjectBuffer/UniformBuffer.h"

#include "Renderer/RendererConfig.h" //update when DirLightObject is update
#include "Renderer/Lights/Lights.h"
#include "Renderer/ObjectBuffer/ShadowMap.h"

#include "Renderer/Shader.h"

#include "Renderer/Skybox.h"  //update when Skybox is update
#include "Util/ModelLoader.h"   //new model loader 

#include "Renderer/ObjectBuffer/Framebuffer.h"

#include "Geometry/Plane.h"

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
	void PreUpdateGPUUniformBuffers(Camera& cam); //camera ubo

	//Pre-Rendering
	void BuildSceneEntities();
	std::vector<std::weak_ptr<Entity>> BuildVisibleEntities(const std::vector<std::shared_ptr<Entity>>& parent_entity);
	void BuildEntitiesWithRenderMesh(const std::shared_ptr<Entity>& parent_entity);
	void BuildEntitiesWithRenderMesh(const std::weak_ptr<Entity>& parent_entity);
	void BuildOpacityTransparencyFromRenderMesh(const std::vector<std::weak_ptr<Entity>>& renderable_list);
	void BuildSceneEntitiesViaOpacityTransparency(const std::shared_ptr<Entity>& parent_entity);
	void SortByViewDistance(std::vector<std::weak_ptr<Entity>>& sorting_list);
	void ShadowPass(Shader& depth_shader);

	//Scene Render
	void PostUpdateGPUUniformBuffers(); //light ubo after scene is sorted & light pass
	void DrawScene(Shader& main_shader);

	//Scene Post Render
	void PostProcess();
	//No after Effect at the moment 

	//After Render 
	void SceneDebugger();
	void DebugEntitiesPos(Entity& enitity);
	//void AfterRender(); //<------------- For clean ups that is required after rendering 

	//scene special data
	bool enableSceneShadow = true;


	//post process parameters
	float gamma = 1.80f;
	float hdrExposure = 1.0f;
	bool m_DoHDR = false;


	//Resource
	Framebuffer m_SceneScreenFBO;
	//Testing Test MRT (multi render target for GBuffer)
	MRTFramebuffer m_TestGBuffer;
	Shader m_TestGBufferShader;
	Shader m_PostImgShader;
	std::shared_ptr<Mesh> m_QuadMesh;
	//Later store entities as enitity but sorted list should be a rendering data list (Mesh,Material,World Transform)
	std::vector<std::shared_ptr<Entity>> m_SceneEntities;
	std::vector<std::weak_ptr<Entity>> m_SceneEntitiesWcRenderableMesh;
	std::vector<std::weak_ptr<Entity>> m_OpaqueEntites;
	std::vector<std::weak_ptr<Entity>> m_TransparentEntites;

	Shader m_SceneShader;//std::vector<std::shared_ptr<Shader>> m_SceneShaders;
	Shader m_ShadowDepthShader;  //this is not scene deoth shader
	Shader m_SkyboxShader;
	std::vector<std::shared_ptr<Material>> m_SceneMaterials;
	Skybox m_Skybox;

	Camera m_TopDownCamera;
	Framebuffer m_TopDownFBO;

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
	CRRT::ModelLoader m_NewModelLoader;
	unsigned int m_PrevViewWidth, m_PrevViewHeight;


	//UniformBuffers
	UniformBuffer m_CamMatUBO;
	UniformBuffer m_LightDataUBO;
	UniformBuffer m_EnviUBO;


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
	//need to take this out later
	void ResizeBuffers(unsigned int width, unsigned int height);

	void MaterialShaderBindHelper(Material& mat, Shader& shader);

	//UIs
	void MainUI();
	void EnititiesUI();
	void EntityDebugUI(Entity& entity);
	void MaterialsUI();
	void EditTopViewUI();
	void TestMRT_GBufferUI();


	void EntityModelMaterial(const Entity& entity);
};