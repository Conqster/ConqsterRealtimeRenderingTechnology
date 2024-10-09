#pragma once
#include <vector>
#include "Lights.h"

#include <memory>

class LightManager
{
private:
	std::vector<std::shared_ptr<Light>> m_Lights;

	std::vector<std::weak_ptr<DirectionalLight>> m_DirectionalLight;
	std::vector<std::weak_ptr<PointLight>> m_PointLight;
	std::vector<std::weak_ptr<SpotLight>> m_SpotLight;
public:
	LightManager();
	~LightManager();

	void Add(std::shared_ptr<Light> light);
	void Clean();

	inline const std::vector<std::shared_ptr<Light>>& GetLights() const { return m_Lights; }

	inline const std::vector<std::weak_ptr<DirectionalLight>>& GetDirLights() const { return m_DirectionalLight; }
	inline const std::vector<std::weak_ptr<PointLight>>& GetPointLights() const { return m_PointLight; }
	inline const std::vector<std::weak_ptr<SpotLight>>& GetSpotLights() const { return m_SpotLight; }
};