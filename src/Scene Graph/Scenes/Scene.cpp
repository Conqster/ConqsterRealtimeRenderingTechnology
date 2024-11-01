#pragma once
#include "Scene.h"
#include "Renderer/Renderer.h"

#include "Util/FilePaths.h"


void Scene::SetWindow(Window* window)
{
	this->window = window;
}


void Scene::OnInit(Window* window)
{
	this->window = window;
	GLCall(glViewport(0, 0, window->GetWidth(), window->GetHeight()));

	blank_tex = new Texture(FilePaths::Instance().GetPath("blank-image"));
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

	if (blank_tex)
		blank_tex->Clear();

	//for now hard reset 
	GLCall(glDisable(GL_DEPTH_TEST));
	GLCall(glDisable(GL_CULL_FACE));

}
