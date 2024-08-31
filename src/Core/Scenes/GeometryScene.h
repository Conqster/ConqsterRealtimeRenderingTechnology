#pragma once
#include "Scene.h"

#include "Graphics/Shader.h"
#include "Graphics/Meshes/SquareMesh.h"

#include "Graphics/ObjectBuffer/UniformBuffer.h"

#include "Graphics/Texture.h"


class GeometryScene : public Scene
{
public:
	GeometryScene() = default;

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

	~GeometryScene();

private:
	void CreateObjects();

	Shader m_Shader;
	SquareMesh m_SquareMesh;

	UniformBuffer m_CameraMatUBO;

	float length = 0.1f;

	//Ground 
	Shader m_GroundShader;
	Texture* m_GroundTex;
	glm::vec3 ground_pos = glm::vec3();
	float ground_rot = 90.0f;
	glm::vec3 ground_scale = glm::vec3(1.0);
	
};