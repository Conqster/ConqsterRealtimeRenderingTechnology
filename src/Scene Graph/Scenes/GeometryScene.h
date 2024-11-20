#pragma once
#include "Scene.h"

#include "Renderer/Renderer.h"
#include "Renderer/Meshes/Mesh.h"

#include "Renderer/ObjectBuffer/UniformBuffer.h"


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
	Mesh m_SquareMesh;

	
	bool debugSquare;

	UniformBuffer m_CameraMatUBO;

	float length = 0.1f;

	//Ground 
	Shader m_GroundShader;
	Texture* m_GroundTex;
	glm::vec3 ground_pos = glm::vec3();
	float ground_rot = 90.0f;
	glm::vec3 ground_scale = glm::vec3(1.0);


	//sphere
	Mesh sphere;
	Texture* sphereTex;
	Shader sphereNormDebugShader;

	float normDebugLength = 0.2f;
	glm::vec3 normDebugColour = glm::vec3(1.0f, 0.0f, 1.0f);
	bool useDebugColour = true;

	//test cube
	class Mesh testCube;
	
};