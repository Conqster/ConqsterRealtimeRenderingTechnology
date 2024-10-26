#pragma once
#include "Scene Graph/Scenes/Scene.h"
#include "Scene Graph/Scenes/SceneManager.h"
#include "Window.h"

#include "Util/GameTime.h"

#include "UIManager.h"
#include <functional>


enum State
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

	std::function<void()> m_MenubarCallback;
	std::string m_UIFont = "Roboto-Bold";
	UIManager* m_UI = nullptr;

	bool m_Running = false;
	bool m_UseFullScreen = true;

	State m_GameState;
public:
	Game() = default;

	void OnStart();
	void SetMenubarCallback(const std::function<void()>& menubar) { m_MenubarCallback = menubar; }
	void CloseProgram() { m_Running = false; }
	void OnLoadSceneUI(const char* label, bool* open_win, bool can_load = false);
	void StatsUI();
	void CameraStatsUI();
	void WindowStatsUI();
	void AllStatsTab();
	void ChangeUIFont();
	void Run();
	void OnEnd();

	void Input();

	~Game() = default;

private: 
	GameTime m_Time;
};
