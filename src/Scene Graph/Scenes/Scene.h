#pragma once
#include "GL/glew.h"
#include "Camera.h"
#include "Window.h"


#include "Renderer/SceneRenderer.h"

#include "Renderer/ObjectBuffer/Framebuffer.h"
#include "Renderer/Skybox.h"
#include "Renderer/Lights/Lights.h"

#include "Renderer/ObjectBuffer/UniformBuffer.h"

#include "Renderer/RendererConfig.h"
#include "Renderer/ObjectBuffer/ShadowMap.h"

struct LearnVertex
{
	unsigned int VAO;
	unsigned int VBO;
};

enum RenderingPath
{
	Forward,
	Deferred
};

class Entity;
class MRTFramebuffer;
class Material;
class Scene
{
public:
	Scene() = default;
	virtual void SetWindow(Window* window);
	inline Camera* const GetCamera() const { return m_Camera; }

	virtual void OnInit(Window* window);
	virtual void OnUpdate(float delta_time);
	virtual void OnRender();
	virtual void OnRenderUI();

	virtual void OnSceneDebug();

	virtual void OnDestroy();

	virtual ~Scene() = default;
protected:
	Camera* m_Camera = nullptr;
	Window* window = nullptr;


	CRRT::SceneRenderer m_SceneRenderer;

	glm::vec3 m_ClearScreenColour = glm::vec3(0.1f, 0.1f, 0.1f);
	class Texture* blank_tex = nullptr;

	//flags 
	bool b_EnableSkybox;
	bool b_ResortTransparency = false;
	bool b_RebuildTransparency = false;
	bool b_FrameHasShadow = false;
	bool b_EnableShadow = false;
	bool b_DebugScene = false;

	//Default Scene Properties
	float m_SkyboxInfluencity = 0.2f;
	float m_SkyboxReflectivity = 0.8f;
	//Scene Config
	RenderingPath m_RenderingPath = Forward;
	float m_FrameCount = 0;
	//util
	int m_PrevViewWidth;
	int m_PrevViewHeight;

	void DefaultSetup();
	void DefaultSceneRendering();

	//Buffers 
	void PreUpdateGPUUniformBuffers(Camera& cam);
	void IntermidateUpdateGPUUniformBuffers();


	//Shading Paths 
	void ForwardShading(std::vector<std::weak_ptr<RenderableMesh>>& opaque_entities, std::vector<std::weak_ptr<RenderableMesh>>& transparent_entities);
	void DeferredShading(std::vector<std::weak_ptr<RenderableMesh>>& opaque_entities, std::vector<std::weak_ptr<RenderableMesh>>& transparent_entities);


	//Passes
	void OpaquePass(Shader& o_shader, std::vector<std::weak_ptr<RenderableMesh>>& o_enitites);
	void TransparencyPass(Shader& t_shader, std::vector<std::weak_ptr<RenderableMesh>>& t_entities);
	void GBufferPass(Shader& g_shader, std::vector<std::weak_ptr<RenderableMesh>>& g_entities);
	void DeferredLightingPass(Shader& d_shader, MRTFramebuffer& g_render_targets);
	void DefaultShadowPass(Shader& sh_depht_shader, std::vector<std::shared_ptr<RenderableMesh>>& entities);

	//Default Resources
	std::vector<std::shared_ptr<Entity>> m_entities;
	std::vector<std::shared_ptr<RenderableMesh>> def_RenderableEntities;
	std::vector<std::weak_ptr<RenderableMesh>> def_OpaqueEntities;
	std::vector<std::weak_ptr<RenderableMesh>> def_TransparentEntities;
	Shader def_ForwardShader;
	Shader def_DeferredShader;
	Shader def_GBufferShader;
	MRTFramebuffer def_GBuffer;
	std::shared_ptr<Mesh> def_QuadMesh;	
	Skybox def_Skybox;
	Shader def_SkyboxShader;
	Shader def_ShadowDepthShader;
	UniformBuffer def_CamMatUBO;
	UniformBuffer m_LightDataUBO;
	UniformBuffer m_EnviUBO;
	std::shared_ptr<Material> def_MeshMaterial;


	////Light Resource
	static const int MAX_POINT_LIGHT = 1000;
	static const int MAX_POINT_LIGHT_SHADOW = 10;
	DirectionalLight def_DirLight;
	std::vector<PointLight> def_PtLights;
	ShadowConfig def_SceneShadowConfig;
	DirShadowCalculation def_DirShadowConfig;
	ShadowMap def_DirDepthMap;
	std::vector<ShadowCube>def_PtDepthCubes;

	//Utilites 
	void DefMaterialShaderBindHelper(Material& mat, Shader& shader);
	void BuildRenderableMeshes(const std::shared_ptr<Entity>& entity);
	void BuildOpaqueTransparency(const std::vector<std::shared_ptr<RenderableMesh>>& renderable_entities);
	void SortByViewDistance(std::vector<std::weak_ptr<RenderableMesh>>& sorting_list);

	//setters
	void SetQuadMesh(std::shared_ptr<Mesh>& mesh) { def_QuadMesh = mesh; }

	void UpdateShadersUniformBuffers();

	void RefreshFrame();
	void ResizeBuffers(unsigned int width, unsigned int height);


	void DebugDisplayDirectionalLightUIPanel(unsigned int scale = 1);



	//UIs
	void SceneDefaultUI();
	void DefaultMainUI();
	void DefaultGBufferDisplayUI();

	//Uis Panels
	void WindowInfoUIPanel();
	void CameraUIPanel();
	void ScenePropUIPanel();

	//Sub trees 
	void Ext_MainUI_LightTree();
	void Ext_MainUI_SceneTree();
};
