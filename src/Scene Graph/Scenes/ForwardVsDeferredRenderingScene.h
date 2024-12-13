#pragma once
#include "Scene.h"

#include "Renderer/Shader.h"
#include "Renderer/ObjectBuffer/UniformBuffer.h"

#include "Util/ModelLoader.h"

#include "Renderer/RendererConfig.h" //update when DirLightObject is update
#include "Renderer/Lights/Lights.h"
#include "Renderer/ObjectBuffer/ShadowMap.h"

#include "Renderer/Skybox.h"

#include "Renderer/ObjectBuffer/Framebuffer.h"
#include "Util/ShaderHotReload.h"


enum RenderingPath
{
	Forward,
	Deferred
};

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
	void SetRenderingConfiguration(const struct SceneRenderingConfig& scene_render_config);
	void CreateLightsAndShadowDatas(const struct SceneLightData& light_data, const struct SceneShadowData& shadow_data);
	void CreateEntities(const struct SceneData& scene_data);  // Loads (Environment, Entities, materials 
	void CreateGPUDatas();

	//experimenting 
	SceneData LoadSceneFromFile(); //Loads (Lights, shadows, 
	void SerialiseScene();


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
	std::vector<std::weak_ptr<Entity>> m_OpaqueEntities;
	std::vector<std::weak_ptr<Entity>> m_TransparentEntities;
	Shader m_ShadowDepthShader;  //this is not scene deoth shader
	Skybox m_Skybox;
	Shader m_SkyboxShader;
	//UniformBuffers
	UniformBuffer m_CamMatUBO;
	UniformBuffer m_LightDataUBO;
	UniformBuffer m_EnviUBO;
	//GBuffer (Light accumulation, Diffuse, Specular, Normal) 
	//Using (Base Colour, Normal, Position, Depth)
	MRTFramebuffer m_GBuffer;
	Shader m_GBufferShader;
	Shader m_DeferredShader;
	std::shared_ptr<Mesh> m_QuadMesh;
	ShaderHotReload  m_ShaderHotReload;
	

	//Utilities
	CRRT::ModelLoader m_NewModelLoader;
	glm::vec3 m_PtOrbitOrigin = glm::vec3(0.0f, 220.0f, 0.0f);
	float m_SpawnZoneRadius = 310.0f;
	float m_DesiredDistance = 200.0f;//based on entity_extra_scaleby
	float m_OrbitSpeed = 20.0f;
	int m_PrevViewHeight;
	int m_PrevViewWidth;
	unsigned int frames_count = 0;

	//flag
	bool b_DebugScene = false;
	bool b_DebugPointLightRange = false;
	bool b_rebuild_transparency = false;
	bool b_FrameAsShadow = false;

	//Scene Render Construction
	void BuildRenderableMeshes(const std::shared_ptr<Entity>& entity);
	void BuildOpaqueTransparency(const std::vector<std::weak_ptr<Entity>>& renderable_entities);
	void SortByViewDistance(std::vector<std::weak_ptr<Entity>>& sorting_list);

	//Uniform buffer update
	void PreUpdateGPUUniformBuffers(Camera& cam); //camera ubo
	void PostUpdateGPUUniformBuffers(); //light ubo after scene is sorted & light pass

	//Rendering/Shading Path
	RenderingPath m_RenderingPath = Forward;
	void ForwardRenderingPath();
	void DeferredRenderingPath();

	//Passes 
	void OpaquePass(Shader& main_shader, const std::vector<std::weak_ptr<Entity>> opaque_entities);
	void TransparencyPass(Shader& main_shader, const std::vector<std::weak_ptr<Entity>> transparent_entities);
	void GBufferPass();
	void DeferredLightingPass();
	void ShadowPass(Shader& depth_shader, const std::vector<std::weak_ptr<Entity>> renderable_meshes);

	void SceneDebugger();


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
	static const int MAX_POINT_LIGHT = 1000;
	static const int MAX_POINT_LIGHT_SHADOW = 10;
	std::vector<PointLight> m_PtLights;
	unsigned int m_PtLightCount = 0;
	ShadowConfig m_PtShadowConfig;
	std::vector<ShadowCube> m_PtDepthMapCubes;



	//------------------------------Utility functions------------------------/
	void ResizeBuffers(unsigned int width, unsigned int height);

	void MaterialShaderBindHelper(Material& mat, Shader& shader);
	bool AddPointLight(glm::vec3 location, glm::vec3 colour);
	void ResetSceneFrame();

	int UpdateLightCount(int step, int progression_base = 2, int scale_factor = 1);


	//UIs
	void MainUI();
	void ExternalMainUI_LightTreeNode();
	void ExternalMainUI_SceneDebugTreeNode();
	void EnititiesUI();
	void EntityDebugUI(Entity& entity);
	void MaterialsUI();
	void EntityModelMaterial(const Entity& entity);
	void GBufferDisplayUI();
};