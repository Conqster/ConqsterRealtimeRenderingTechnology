#pragma once
#include "Scene.h"

#include "Renderer/Shader.h"
#include "Renderer/ObjectBuffer/UniformBuffer.h"

#include "Renderer/Lights/Lights.h"
#include "Renderer/ObjectBuffer/Framebuffer.h"

#include "Renderer/Skybox.h"
#include "Util/ShaderHotReload.h"


struct BaseMaterial;
struct ImVec2;
class LightingModelingScene : public Scene
{
public:
	LightingModelingScene() = default;

	virtual void SetWindow(Window* window);

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

private:
	void CreateObjects();

	void UpdateCameraUBO(Camera& cam, float aspect_ratio);

	void UpdateShaders();

	void MainSceneUIEditor();
	void Ext_LightingUIPanelSection();
	void Ext_EnvPropUIPanelSection();

	void SceneObjectTransformEditorUI();
	bool Ext_TransformEditorPanel(glm::mat4& transform);

	void MaterialEditorUI();
	bool Ext_QuatEditorPanel(glm::mat4& transform, ImVec2 top_left, ImVec2 size);
	void MaterialShaderHelper(Shader& shader, const BaseMaterial& mat);

	void DebugScene();

	Shader m_Shader;
	UniformBuffer m_CameraUBO;

	DirectionalLight m_DirLight;
	PointLight m_PointLight;
	SpotLight m_SpotLight;

	//scene object 
	std::shared_ptr<Mesh> m_SphereMesh;
	std::shared_ptr<Mesh> m_QuadMesh;
	std::shared_ptr<Mesh> m_CubeMesh;
	std::shared_ptr<Mesh> m_ConeMesh;

	std::shared_ptr<BaseMaterial> m_SphereMaterial;
	std::shared_ptr<BaseMaterial> m_QuadMaterial;
	std::shared_ptr<BaseMaterial> m_CubeMaterial;

	std::vector<glm::mat4> m_SphereTransform;
	std::vector<glm::mat4> m_PlaneTransform;
	std::vector<glm::mat4> m_CubeTransform;
	std::vector<glm::mat4> m_ConeTransform;


	//environment
	bool b_EnableSkybox = false;
	bool b_PhongRendering = false;
	Shader m_SkyboxShader;
	Skybox m_Skybox;


	ShaderHotReload  m_ShaderHotReload;



	//Helpers
	Camera m_MaterialPreviewCamera;
	Framebuffer m_MaterialPreviewFBO;
	glm::mat4 m_PreviewTransform = glm::mat4(1.0f);
	std::weak_ptr<BaseMaterial> m_PreviewMaterial;
	bool b_MaterialPreviewLighting = true;

	bool b_DebugScene = false;
};