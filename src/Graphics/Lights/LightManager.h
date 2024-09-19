#pragma once
#include <vector>
#include "NewLights.h"

#include <memory>

class LightManager
{
private:
	std::vector<std::shared_ptr<NewLight>> m_Lights;

	std::vector<std::weak_ptr<NewDirectionalLight>> m_DirectionalLight;
	std::vector<std::weak_ptr<NewPointLight>> m_PointLight;
	std::vector<std::weak_ptr<NewSpotLight>> m_SpotLight;
public:
	LightManager();
	~LightManager();

	void Add(std::shared_ptr<NewLight> light);
	void Clean();

	inline const std::vector<std::shared_ptr<NewLight>>& GetLights() const { return m_Lights; }

	inline const std::vector<std::weak_ptr<NewDirectionalLight>>& GetDirLights() const { return m_DirectionalLight; }
	inline const std::vector<std::weak_ptr<NewPointLight>>& GetPointLights() const { return m_PointLight; }
	inline const std::vector<std::weak_ptr<NewSpotLight>>& GetSpotLights() const { return m_SpotLight; }
};