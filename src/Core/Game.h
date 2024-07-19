#pragma once
#include "Graphics/Renderer.h"
#include "Camera.h"
#include "Util/GameTime.h"

#include "GameObject.h"
#include "Physics/PhysicsWorld.h"

#include "Graphics/MainRenderer.h"
#include "Graphics/LearningRenderer.h"


enum RenderScene
{
	None = -1,
	MainScene = 0,
	Learning = 1,
	Count = 2,
};


class Game
{
public: 
	Game();

	void Start();
	void Update();
	void End();

	bool Running();

	~Game();
private:

	const unsigned int windowWidth = 1920;//2560;
	const unsigned int windowHeight = 1080;  //1440; 

	bool m_Running = false;
	void Input();

	void CreateGameObjects();
	void CreateLights();


	void UIUpdate();


	int selected_light_idx = 0;

	RenderScene renderScene = RenderScene::Learning;
	bool m_RenderMainScene = false;
	///*class*/ MainRenderer m_MainRenderer;
	MainRenderer m_MainRenderer;
	LearningRenderer m_LearningRendering;
	
	class Window* window;
	GameTime m_GameTime;

	PhysicsWorld m_PhysicsWorld;

	std::vector<GameObject*> m_GameObjects;
	Camera m_Camera;


	unsigned int m_SelectedGameobjectidx = 0;
};

