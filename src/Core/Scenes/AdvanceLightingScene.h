#pragma once
#include "Scene.h"

#include "SceneGraph/ModelLoader.h"
#include "Graphics/Shader.h"
#include "SceneGraph/Model.h"
#include "Graphics/ObjectBuffer/UniformBuffer.h"
#include "Graphics/Meshes/SphereMesh.h"
#include "Graphics/Meshes/SquareMesh.h"

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

	ModelLoader modelLoader;

	std::shared_ptr<Model> model_1;
	glm::vec3 model_1Pos = glm::vec3(0.0f);
	float model_1Scale = 10.0f;
	Shader modelShader;

	//debugging
	Shader debugShader;
	float normDebugLength = 0.2f;
	glm::vec3 normDebugColour = glm::vec3(1.0f, 0.0f, 1.0f);
	bool useDebugColour = true;
	bool debugVertexPosColour = false;

	std::shared_ptr<Model> model_2;
	glm::vec3 model_2Pos = glm::vec3(1.0f, 0.0f, 0.0f);
	float model_2Scale = 0.1f;



	UniformBuffer m_CameraMatUBO;

	//sphere
	SphereMesh sphere;
	Texture* sphereTex;

	//ground
	SquareMesh ground;
};
