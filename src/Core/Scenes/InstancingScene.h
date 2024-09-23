#pragma once
#include "Scene.h"

#include "Graphics/Shader.h"
#include "Graphics/Meshes/SquareMesh.h"
#include "Graphics/Meshes/SphereMesh.h"

#include "Graphics/ObjectBuffer/UniformBuffer.h"

#include "Graphics/Skybox.h"

#include "SceneGraph/ModelLoader.h"
#include "SceneGraph/Model.h"

class InstancingScene : public Scene
{
public:
	InstancingScene() = default;


	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

	~InstancingScene();

private:
	void CreateObjects();

	Shader m_Shader;
	SquareMesh m_SquareMesh;

	UniformBuffer m_CameraMatUBO;


	bool debugColour;
	float squareScale = 1.0f;
	float offsetInstance = 2.0f;
	int instanceCount = 5;


	//Skybox
	Skybox m_Skybox;

	//Sphere
	SphereMesh sphereMesh;


	//Load Model 
	std::shared_ptr<Model> sampleModel;
	Texture* samplePlainTexture;
	float modelScale = 1.0f;
	//bool noTexture = true;
	ModelLoader modelLoader;
	Shader modelShader;
	Shader debugNorShader;

	//Debug properites
	float normDebugLength;
	glm::vec3 normDebugColour;
	bool useDebugColour;

};