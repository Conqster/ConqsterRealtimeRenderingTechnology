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

	virtual void OnSceneDebug() override;

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

	//Scene Resources
	//std::shared_ptr<Material> defaultFallBackMaterial; //shared_ptr, just in case to take ownership if other ref gets deleted
	std::vector<std::shared_ptr<Entity>> m_SceneEntities;
	Shader m_ShadowDepthShader;  //this is not scene deoth shader
	int m_PrevViewWidth;
	int m_PrevViewHeight;
	

	//Utilities
	CRRT::ModelLoader m_NewModelLoader;
	glm::vec3 m_PtOrbitOrigin = glm::vec3(0.0f, 220.0f, 0.0f);
	float m_SpawnZoneRadius = 310.0f;
	float m_DesiredDistance = 200.0f;//based on entity_extra_scaleby
	float m_OrbitSpeed = 20.0f;

	bool m_DebugScene = false;
	bool m_DebugPointLightRange = false;

	//Begin Scene Render
	void BeginRenderScene();
	void ShadowPass(Shader& depth_shader, const std::vector<std::weak_ptr<Entity>> renderable_meshes);


	
	void ResetSceneFrame();

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
	static const int MAX_POINT_LIGHT = 1000;
	static const int MAX_POINT_LIGHT_SHADOW = 10;
	std::vector<PointLight> m_PtLights;
	unsigned int m_PtLightCount = 0;
	ShadowConfig m_PtShadowConfig;
	std::vector<ShadowCube> m_PtDepthMapCubes;

	bool m_FrameAsShadow = false;


	//------------------------------Utility functions------------------------/
	//need to take this out later
	void ResizeBuffers(unsigned int width, unsigned int height);

	bool AddPointLight(glm::vec3 location, glm::vec3 colour);


	//UIs
	void MainUI();
	void ExternalMainUI_LightTreeNode();
	void ExternalMainUI_SceneDebugTreeNode();
	void EnititiesUI();
	void EntityDebugUI(Entity& entity);
	void MaterialsUI();
	void EntityModelMaterial(const Entity& entity);

	void GBufferDisplayUI();
	//void ScreenFBODisplayUI();
};



//rendeable mesh
//mesh data 
//transform 
//material