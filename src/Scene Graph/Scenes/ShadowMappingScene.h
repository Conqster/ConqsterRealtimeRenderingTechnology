#pragma once
#include "Scene.h"

#include "Renderer/Shader.h"
#include "Renderer/ObjectBuffer/UniformBuffer.h"
#include "Renderer/Lights/Lights.h"
#include "Renderer/Skybox.h"

#include "Util/ShaderHotReload.h"

#include "Renderer/ObjectBuffer/ShadowMap.h"
#include "Renderer/RendererConfig.h"



struct BaseMaterial;
class ShadowMappingScene : public Scene
{
public:
	ShadowMappingScene() = default;

	virtual void SetWindow(Window* window);
	inline Camera* const GetCamera() const { return m_Camera; }

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;
private:
	void CreateObjects();
	void DrawObjects(Shader& shader, bool only_geometry = false);
	void ShadowPass();

	//Helper 
	void UpdateCameraUBO(Camera& cam, float aspect_ratio);
	void MaterialShaderHelper(Shader& shader, const BaseMaterial& mat);
	void UpdateShaders();
	void DebugScene();

	void MainSceneUIEditor(bool& update_dir_shadow);
	void Ext_EnvPropUIPanelSection();
	void Ext_LightingUIPanelSection(bool& update_dir_shadow);

	void SceneObjectTransformEditorUI();
	bool Ext_TransformEditorPanel(glm::mat4& transform);

	void ShadowPropEditorUI(bool& update_dir_shadow);

	//scene object 
	std::shared_ptr<Mesh> m_SphereMesh;
	std::shared_ptr<Mesh> m_QuadMesh;
	std::shared_ptr<Mesh> m_CubeMesh;

	std::vector<glm::mat4> m_SphereTransform;
	std::vector<glm::mat4> m_PlaneTransform;
	std::vector<glm::mat4> m_CubeTransform;

	std::shared_ptr<BaseMaterial> m_SphereMaterial;
	std::shared_ptr<BaseMaterial> m_QuadMaterial;
	std::shared_ptr<BaseMaterial> m_CubeMaterial;

	Shader m_Shader;
	Shader m_ShadowDepthShader;
	UniformBuffer m_CameraUBO;

	DirectionalLight m_DirLight;
	PointLight m_PointLight;

	std::shared_ptr<Mesh> m_ModelMesh;
	std::vector<glm::mat4> m_ModelTransform;

	//environment
	bool b_EnableSkybox = false;
	Shader m_SkyboxShader;
	Skybox m_Skybox;

	//shadow
	ShadowMap m_DirShadowMap;
	DirShadowCalculation m_DirLightShadowProp;

	ShadowCube m_PointShadowCubeMap;
	ShadowConfig m_PointShadowConfig;


	////////////////
	// Utilities 
	////////////////
	bool b_DebugScene = false;
	bool b_FrameHasShadow = false;

	ShaderHotReload m_ShaderHotReload;
	
	//Animate Light Prop
	struct AnimateObjects
	{
		glm::vec3 orbitOrigin = glm::vec3(0.0f, 3.0f, 0.0f);
		//angular speed
		float omega = 8.0f; 
		float orbitRadius = 10.0f;
		bool debugGizmos = false;

		//Helper 
		void Update(glm::vec3& pos, float& theta, float dt)
		{
			theta += glm::radians(dt * omega);

			pos.x = orbitOrigin.x + orbitRadius * glm::cos(theta);
			pos.z = orbitOrigin.z + orbitRadius * glm::sin(theta);
		}
	};

	AnimateObjects m_PtLightAnimateProp;
	std::vector<float> m_AnimateObjectInstanceAngle;
	void AnimatedObjectPropertiesEditorUI();

};
