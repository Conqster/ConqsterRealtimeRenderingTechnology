#pragma once
#include "Scene.h"

//Scene::Scene()
//{
//}
void Scene::SetWindow(Window* window)
	{
		this->window = window;
	}


	void Scene::OnInit(Window* window)
	{
		this->window = window;
	}

	void Scene::OnUpdate(float delta_time)
	{

	}

	void Scene::OnRender()
	{
		
	}

	void Scene::OnRenderUI()
	{
		
	}

	void Scene::OnDestroy()
	{
		delete m_Camera;
		m_Camera = nullptr;

		window = nullptr;

	}
