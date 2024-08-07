#include "LightManager.h"

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
}

void LightManager::Add(std::shared_ptr<NewLight> light)
{
	m_Lights.push_back(light);
	//printf("Added a new light!!!!!!\n");

	LightType type = light->type;

	switch (type)
	{
	case lightType_DIRECTIONAL:
		{
			std::shared_ptr<NewDirectionalLight> dl = std::dynamic_pointer_cast<NewDirectionalLight>(light);
			if(dl)
				m_DirectionalLight.push_back(dl);
			return;
		}
	case lightType_POINT:
		{
			std::shared_ptr<NewPointLight> pl = std::dynamic_pointer_cast<NewPointLight>(light);
			if (pl)
				m_PointLight.push_back(pl);
			return;
		}
	case lightType_SPOT:
		{
			std::shared_ptr<NewSpotLight> sl = std::dynamic_pointer_cast<NewSpotLight>(light);
			if(sl)
				m_SpotLight.push_back(sl);
			return;
		}
	default:

		printf("Light type does not exist!!!!!!!\n");
		break;
	}
}
