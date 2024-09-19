#pragma once

#include "Scenes/Scene.h"

#include "Graphics/Meshes/CubeMesh.h"
#include "Graphics/Shader.h"

#include "Graphics/ObjectBuffer/UniformBuffer.h"
#include "Graphics/ObjectBuffer/Framebuffer.h"
#include "Graphics/ObjectBuffer/MSAA.h"


class AntiAliasingScene : public Scene
{
public:
	AntiAliasingScene() = default;

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

	~AntiAliasingScene();

private:
	void CreateObjects();

	glm::vec3 debugColour;
	bool useDebugColour;

	Shader cubeShader;
	CubeMesh cube;

	float scaleBy = 1.0f;

	LearnVertex m_Quad; //for screen frame tex
	Shader screenShader;

	//debugging screen space texture
	bool m_DebugScreenTex;
	glm::vec3 m_DebugScreenTexColour;


	MSAA m_MSAA;
	MSAA m_MSAA2;
	bool m_DoMSAA = true;
	bool m_DoMSAA2 = false;
	int m_SampleCount = 8;
	
};
