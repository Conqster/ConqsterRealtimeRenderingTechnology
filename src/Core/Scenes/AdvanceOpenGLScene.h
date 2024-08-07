#pragma once
#include "Scene.h"

#include "Graphics/NewShader.h"
#include "LearningOpenGLModel.h"



//forward declare
class Texture;

class AdvanceOpenGLScene : public Scene
{
public: 
	AdvanceOpenGLScene() = default;

	virtual void OnInit() override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

	~AdvanceOpenGLScene();


private:
	glm::vec3 m_ClearScreenColour = glm::vec3(0.1f, 0.1f, 0.1f);


	learning::ObjectVertex m_Cube;
	learning::ObjectVertex m_Plane;

	Texture* metalTex;
	Texture* marbleTex;

	NewShader objectShaders;
	NewShader outlineShader;
	bool doDepthTest = true;
	bool secondpass_stencil = true;
	glm::vec4 outlineColour = glm::vec4(0.4f, 0.0f, 0.8f, 1.0f);
	float outlineThickness = 1.1f;
	float brightness = 1.0f;

	class SquareMesh* grassMesh = nullptr;
	Texture* grassTexture;
	NewShader grassShader;
	glm::vec3 grassPos = glm::vec3(0.0f);
	glm::vec3 grassRotAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	float grassRot = 0.0f;

	void CreateObjects();
};