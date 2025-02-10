#pragma once
#include "Scene Graph/Scenes/Scene.h"

#include <string>
#include <unordered_map>
#include <memory>



//Reduced the startup time from 1000s down to 300s 
//as scene is not load on start up 
class SceneManager
{
public:

	static inline SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}


	template<class T>
	void RegisterNewScene(const std::string& name);
	Scene* LoadScene(const std::string name, Window* window);
	//expense to call
	int GetCurrentLoadedScene();
	const char** ScenesByNamePtr();
	inline int SceneCount() { return static_cast<int>(m_sceneNamesPtr.size()); }

	~SceneManager();

private:
	std::unordered_map<std::string, std::unique_ptr<Scene>> m_SceneRegister;
	std::vector<std::string> m_sceneNames;
	std::vector<const char*> m_sceneNamesPtr; //Cache pointer to scene names for ImGui use
	bool sceneNamesUpdated = false;
	std::string m_LastLoadedScene;

	void Cleanup();
};

template<class T>
inline void SceneManager::RegisterNewScene(const std::string& name)
{
	m_sceneNames.push_back(name);
	m_SceneRegister[name] = std::make_unique<T>();
	sceneNamesUpdated = true;
}
