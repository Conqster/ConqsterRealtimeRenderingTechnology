#pragma once
#include "Scene.h"

#include "LearningOpenGLModel.h"
#include "Graphics/Shader.h"

#include "Graphics/ObjectBuffer/UniformBuffer.h"


class FaceCullingScene : public Scene
{
public:
	FaceCullingScene() = default;


	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

	~FaceCullingScene() override;


private:
	learning::ObjectVertex m_Cube;
	class Texture* crateTex;

	Shader m_DefaultShader;
	Shader m_Obj1Shader;
	Shader m_Obj2Shader;
	Shader m_Obj3Shader;

	void CreateObjects();

	UniformBuffer m_CameraMatUBO;
};