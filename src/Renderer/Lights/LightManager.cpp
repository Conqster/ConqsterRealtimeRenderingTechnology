#include "LightManager.h"

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
	Clean();
}

void LightManager::Add(std::shared_ptr<Light> light)
{
	m_Lights.push_back(light);

	LightType type = light->type;

	switch (type)
	{
		case LightType::lightType_DIRECTIONAL:
		{
			std::shared_ptr<DirectionalLight> dl = std::dynamic_pointer_cast<DirectionalLight>(light);
			if (dl)
				m_DirectionalLight.push_back(dl);
			return;
		}
		case LightType::lightType_POINT:
		{
			std::shared_ptr<PointLight> pl = std::dynamic_pointer_cast<PointLight>(light);
			if (pl)
				m_PointLight.push_back(pl);
			return;
		}
		case LightType::lightType_SPOT:
			{
				std::shared_ptr<SpotLight> sl = std::dynamic_pointer_cast<SpotLight>(light);
				if(sl)
					m_SpotLight.push_back(sl);
				return;
			}
		default:

			printf("Light type does not exist!!!!!!!\n");
			break;
	}
}

void LightManager::Clean()
{
	m_Lights.clear();
}
