#include "SceneManager.h"



Scene* SceneManager::LoadScene(const std::string name, Window* window)
{
	auto it = m_SceneRegister.find(name);

	if (it != m_SceneRegister.end())
	{
		it->second->OnInit(window);
		return it->second.get();
	}

	//Log
	printf("Scene '%s' does not exist or is not registered!!!\n", name);

	return nullptr;
}

const char** SceneManager::ScenesByNamePtr()
{
	if (sceneNamesUpdated)
	{
		m_sceneNamesPtr.clear();
		for (const auto& name : m_sceneNames)
			m_sceneNamesPtr.push_back(name.c_str());
		sceneNamesUpdated = false;
	}

	return m_sceneNamesPtr.data();
}

SceneManager::~SceneManager()
{
	Cleanup();
}

void SceneManager::Cleanup()
{
	m_SceneRegister.clear();
	m_sceneNames.clear();
	m_sceneNamesPtr.clear();
}
