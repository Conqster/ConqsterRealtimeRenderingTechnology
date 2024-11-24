#pragma once
#include "Scene.h"

#include "Renderer/Shader.h"
#include "Renderer/ObjectBuffer/UniformBuffer.h"

#include "Util/ModelLoader.h"

#include "Renderer/RendererConfig.h" //update when DirLightObject is update
#include "Renderer/Lights/Lights.h"
#include "Renderer/ObjectBuffer/ShadowMap.h"

#include "Renderer/Skybox.h"

class Material;
class Entity;
class ForwardVsDeferredRenderingScene : public Scene
{
public: 
	ForwardVsDeferredRenderingScene() = default;

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


	//Scene Properties 
	bool m_EnableShadows = true;
	bool m_EnableSkybox = false;
	float m_SkyboxInfluencity = 0.2f;
	float m_SkyboxReflectivity = 0.8f;

	//Scene Resources
	Shader m_ForwardShader;
	std::shared_ptr<Material> defaultFallBackMaterial; //shared_ptr, just in case to take ownership if other ref gets deleted
	std::vector<std::shared_ptr<Entity>> m_SceneEntities;
	std::vector<std::weak_ptr<Entity>> m_RenderableEntities;
	Shader m_ShadowDepthShader;  //this is not scene deoth shader
	Skybox m_Skybox;
	Shader m_SkyboxShader;
	//UniformBuffers
	UniformBuffer m_CamMatUBO;
	UniformBuffer m_LightDataUBO;
	UniformBuffer m_EnviUBO;

	//Utilities
	CRRT::ModelLoader m_NewModelLoader;

	//Begin Scene Render
	void BeginRenderScene();
	void PreUpdateGPUUniformBuffers(Camera& cam); //camera ubo
	void ShadowPass(Shader& depth_shader, const std::vector<std::weak_ptr<Entity>> renderable_meshes);

	//Pre-Rendering
	unsigned int frames_count = 0;
	void BuildRenderableMeshes(const std::shared_ptr<Entity>& entity);

	//Scene Render
	void PostUpdateGPUUniformBuffers(); //light ubo after scene is sorted & light pass
	void OpaquePass(Shader& main_shader, const std::vector<std::weak_ptr<Entity>> opaque_entities);
	void SceneDebugger();

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
	//Point Light data
	static const int MAX_POINT_LIGHT = 10;
	PointLight m_PtLights[MAX_POINT_LIGHT];
	unsigned int m_PtLightCount = 0;
	ShadowConfig m_PtShadowConfig;
	std::vector<ShadowCube> m_PtDepthMapCubes;

	float orbit_range = 2.0f;


	//------------------------------Utility functions------------------------/
	//need to take this out later
	//void ResizeBuffers(unsigned int width, unsigned int height);

	void MaterialShaderBindHelper(Material& mat, Shader& shader);

	//UIs
	void MainUI();
	void ExternalMainUI_LightTreeNode();
	void EnititiesUI();
	void EntityDebugUI(Entity& entity);
	void MaterialsUI();
	void EntityModelMaterial(const Entity& entity);
};