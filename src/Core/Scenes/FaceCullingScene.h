#pragma once
#include "Scene.h"

#include "LearningOpenGLModel.h"
#include "Graphics/Shader.h"


class FaceCullingScene : public Scene
{
public:
	FaceCullingScene() = default;


	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

	~FaceCullingScene();


private:
	learning::ObjectVertex m_Cube;
	class Texture* crateTex;

	Shader shader;

	void CreateObjects();
};