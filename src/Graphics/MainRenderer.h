#pragma once

#include "RendererBase.h"

class GameObject;

class MainRenderer : public RendererBase
{
public:

	MainRenderer();
	MainRenderer(unsigned int width, unsigned int height);
	MainRenderer(Window& use_window);

	void AddPointLight(std::unique_ptr<PointLight> point_light);
	void AddSpotLight(std::unique_ptr<SpotLight> spot_light);
	void AddDirectionalLight(std::unique_ptr<DirectionalLight> directional_light);

	void RenderObjects(const std::vector<GameObject*> objects, class Camera& camera);

	inline std::vector<Light*>& const GetLights() { return m_Lights; }

	inline unsigned int const LightsCount() { return m_Lights.size(); }

	inline bool* const UseNewShading() { return &m_UseNewShading; }

	//~MainRenderer();
private: 
	std::vector<std::unique_ptr<PointLight>> m_PointLights;
	std::vector<std::unique_ptr<SpotLight>> m_SpotLights;
	std::vector<std::unique_ptr<DirectionalLight>> m_DirectionLights;
	std::vector<Light*> m_Lights;

	GameObject* m_CubeDebugObject = nullptr;
	bool m_UseNewShading = false;


	void CreateLightDebugObj();

	void ProcessLight();
};