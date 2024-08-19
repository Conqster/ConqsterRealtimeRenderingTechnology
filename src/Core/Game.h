#pragma once
#include "Scenes/Scene.h"
#include "Window.h"

#include "Util/GameTime.h"
#include "UIManager.h"


class Game
{
private:
	Window* m_Window = nullptr;//
	WindowProperties m_WindowProp;//
	Scene* m_CurrentScene;
	UIManager* m_UI = nullptr;

	bool m_Running = false;
	bool m_UseFullScreen = false;
public:
	Game() = default;

	void OnStart();
	void Run();
	void OnEnd();

	void Input();

	~Game() = default;

private: 
	GameTime m_Time;
};
