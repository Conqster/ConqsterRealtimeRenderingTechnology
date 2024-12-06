#pragma once
#include "Scene Graph/Scenes/Scene.h"
#include "Scene Graph/Scenes/SceneManager.h"
#include "Window.h"

#include "Util/GameTime.h"
#include "UIManager.h"



enum class State
{
	INIT,
	LOADSCENE,
	RUNNINGSCENE,
};


class Game
{
private:
	Window* m_Window = nullptr;//
	WindowProperties m_WindowProp;//
	Scene* m_CurrentScene;
	SceneManager* m_SceneManager = nullptr;
	UIManager* m_UI = nullptr;

	bool m_Running = false;
	bool m_UseFullScreen = true;

	State m_GameState;
public:
	Game() = default;

	void OnStart();
	void OnLoadSceneUI(const char* label, bool can_load);
	void Run();
	void OnEnd();

	void Input();

	~Game() = default;

private: 
	GameTime m_Time;
};
