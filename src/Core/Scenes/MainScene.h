#pragma once
#include "Scene.h"

#include "GameObject.h"
#include "Graphics/Lights/LightManager.h"
#include "Graphics/Shader.h"


class MainScene : public Scene
{
public:
	MainScene() = default;

	virtual void SetWindow(Window* window);
	inline Camera* const GetCamera() const { return m_Camera; }

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;
private:
	//TO-DO: Later have a setup light helper class 
	//       As well as creat object 
	void CreateObjects();
	void SetupLights();


	void ProcessLight(Shader& shader);
	void ProcessInput(float delta_time);

	//for now
	Shader m_MainShaderProgram;
	//Window* window = nullptr;

	std::vector<GameObject*> m_GameObjects;
	uint16_t m_CurrentSelectedGameobjectIdx = 0;

	LightManager m_LightManager;
	uint16_t selected_light_idx = 0;

	int g_ObjectSpecularExponent = 32;

	float g_AbientStrength = 1.0f,
		g_DiffuseStrength = 1.0f,
		g_SpecularStrength = 1.0f;
};