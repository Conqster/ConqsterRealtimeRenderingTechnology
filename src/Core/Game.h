#pragma once
#include "Graphics/Renderer.h"
#include "Camera.h"
#include "Util/GameTime.h"

#include "GameObject.h"
#include "Physics/PhysicsWorld.h"

class Game
{
public: 
	Game();

	void Start();
	void Update();
	void End();

	bool Running();
private:

	const unsigned int windowWidth = 1920;//2560;
	const unsigned int windowHeight = 1080;  //1440; 

	bool m_Running = false;
	void Input();

	void CreateGameObjects();


	int selected_light_idx = 0;
	Renderer m_Renderer;
	GameTime m_GameTime;

	PhysicsWorld m_PhysicsWorld;

	std::vector<GameObject*> m_GameObjects;
	Camera m_Camera;


	unsigned int m_SelectedGameobjectidx = 0;
};

