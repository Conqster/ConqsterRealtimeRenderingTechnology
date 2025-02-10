#include "SceneManager.h"



Scene* SceneManager::LoadScene(const std::string name, Window* window)
{
	auto it = m_SceneRegister.find(name);

	if (it != m_SceneRegister.end())
	{
		m_LastLoadedScene = name;
		it->second->OnInit(window);
		return it->second.get();
	}

	//Log
	printf("Scene '%s' does not exist or is not registered!!!\n", name.c_str());

	return nullptr;
}

int SceneManager::GetCurrentLoadedScene()
{
	if (m_LastLoadedScene.empty())
		return -1;
	auto it = std::find(m_sceneNames.begin(), m_sceneNames.end(), m_LastLoadedScene);
	if (it != m_sceneNames.end())
		return std::distance(m_sceneNames.begin(), it);

	return -1;
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
	m_LastLoadedScene.clear();
}
