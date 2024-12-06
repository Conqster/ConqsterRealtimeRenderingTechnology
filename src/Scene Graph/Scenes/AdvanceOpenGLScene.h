#pragma once
#include "Scene.h"

#include "Renderer/Shader.h"



//forward declare
class Texture;

class AdvanceOpenGLScene : public Scene
{
public: 
	AdvanceOpenGLScene() = default;

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

	~AdvanceOpenGLScene();


private:

	LearnVertex m_Cube;
	LearnVertex m_Plane;

	Texture* metalTex;
	Texture* marbleTex;

	Shader objectShaders;
	Shader outlineShader;
	bool doDepthTest = true;
	bool secondpass_stencil = true;
	glm::vec4 outlineColour = glm::vec4(0.4f, 0.0f, 0.8f, 1.0f);
	float outlineThickness = 1.1f;
	float brightness = 1.0f;

	class std::shared_ptr<Mesh> grassMesh = nullptr;
	//class Mesh* grassMesh = nullptr;
	Texture* grassTexture;
	Shader grassShader;
	glm::vec3 grassPos = glm::vec3(0.0f);
	glm::vec3 grassRotAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	float grassRot = 0.0f;

	void CreateObjects();
};