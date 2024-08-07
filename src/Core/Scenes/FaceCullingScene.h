#pragma once
#include "Scene.h"

#include "LearningOpenGLModel.h"
#include "Graphics/NewShader.h"


class FaceCullingScene : public Scene
{
public:
	FaceCullingScene() = default;


	virtual void OnInit() override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

	~FaceCullingScene();


private:
	glm::vec3 m_ClearScreenColour = glm::vec3(0.1f, 0.1f, 0.1f);


	learning::ObjectVertex m_Cube;
	class Texture* crateTex;

	NewShader shader;

	void CreateObjects();
};