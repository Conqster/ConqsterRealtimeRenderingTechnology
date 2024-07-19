#include "MainRenderer.h"

#include "Meshes/CubeMesh.h"
#include "Texture.h"

#include "GameObject.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

MainRenderer::MainRenderer()
	: RendererBase()
{
	m_SceneName = "Main Scene";
}

MainRenderer::MainRenderer(unsigned int width, unsigned int height)
	:RendererBase(width, height)
{
	m_SceneName = "Main Scene";
}

MainRenderer::MainRenderer(Window& use_window)
	:RendererBase(use_window)
{
	m_SceneName = "Main Scene";
}

void MainRenderer::AddPointLight(std::unique_ptr<PointLight> point_light)
{
	if (m_PointLights.size() > Shader_Constants::MAX_POINT_LIGHTS)
		return;

	m_Lights.push_back(point_light.get());
	m_PointLights.push_back(std::move(point_light));
	//m_PointLights.push_back(light);
	////m_Lights.push_back(light);
	//// m_PointLight.push_back creates a copy of light inside the vector
	//// &m_PointLights.back get the address of this newly created copy, 
	//// which is now managed by the m_PointLights vector 
	//// m_Lights,push_bak(&m_PointLights.back() stors this address in m_Light vector 
	//// 
	//// Why not &light
	//// &light get the address of thye orignial light objcet, 
	//// not the copy inside the m_PointLights vector
	//// iflight is local variable, it will go out of scope and the pointer stored in m_Light vector will be invliad
	////
	//m_Lights.push_back(&m_PointLights.back());
}

void MainRenderer::AddSpotLight(std::unique_ptr<SpotLight> spot_light)
{
	if (m_PointLights.size() > Shader_Constants::MAX_SPOT_LIGHTS)
		return;

	m_Lights.push_back(spot_light.get());
	m_SpotLights.push_back(std::move(spot_light));
}

void MainRenderer::AddDirectionalLight(std::unique_ptr<DirectionalLight> directional_light)
{
	m_Lights.push_back(directional_light.get());
	m_DirectionLights.push_back(std::move(directional_light));
	//m_Lights.push_back(light);
}

void MainRenderer::RenderObjects(const std::vector<GameObject*> objects, Camera& camera)
{
	m_MainShaderProgram.UseShader();
	m_MainShaderProgram.SetUniformMat4f("u_view", camera.CalViewMat());
	m_MainShaderProgram.SetUniformVec3("u_ViewPos", camera.GetPosition());
	m_MainShaderProgram.SetUniform1i("u_UseNew", m_UseNewShading);

	if (m_Lights.size() > 0)
		ProcessLight();



	glm::mat4 gameobject_model;
	for (size_t i = 0; i < objects.size(); i++)
	{
		auto& gameobject = objects[i];
		gameobject_model = glm::mat4(1.0f);
		gameobject_model = glm::translate(gameobject_model, gameobject->worldPos);
		//first 3 == xyz
		gameobject_model = glm::rotate(gameobject_model, glm::radians(gameobject->rotation.w), (glm::vec3)gameobject->rotation);
		gameobject_model = glm::scale(gameobject_model,/* 50.0f **/ gameobject->worldScale);

		m_MainShaderProgram.SetUniformMat4f("u_model", gameobject_model);
		m_MainShaderProgram.UseShader();

		bool debug_mode = false;
		//DEBUGING GAMEOBJECT
		if (gameobject->selected)
		{
			m_MainShaderProgram.SetUniform1i("u_DebugMode", 1);
			//gameobject->Draw(); //solid debug
			gameobject->DebugDraw(); //line debug
			m_MainShaderProgram.SetUniform1i("u_DebugMode", 0);
		}
		else if (debug_mode)
		{
			//gameobject->Draw();
			m_MainShaderProgram.SetUniform1i("u_DebugMode", 1);
			//gameobject->Draw(); //solid debug
			gameobject->DebugDraw(); //line debug
			m_MainShaderProgram.SetUniform1i("u_DebugMode", 0);
		}

		gameobject->Draw();
	}


	//DRAW GRID
	if (false)
	{
		//RenderGrid();
	}


	//////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////POINT LIGHTS DEBUG LOCATION////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	glm::mat4 model(1.0f);
	for (auto& light : m_Lights)
	{
		if (auto a_point_light = dynamic_cast<PointLight*>(light))
		{
			if (!m_CubeDebugObject)
				CreateLightDebugObj();

			model = glm::mat4(1.0f);
			model = glm::translate(model, a_point_light->GetPosition());
			model = glm::scale(model, m_CubeDebugObject->worldScale);
			m_MainShaderProgram.UseShader();
			m_MainShaderProgram.SetUniformMat4f("u_model", model);
			m_CubeDebugObject->Draw();
			//m_GlobalShaderProgram.SetUniform1i("u_DebugMode", 1);
			//m_CubeDebugObject->DebugDraw();
			//m_GlobalShaderProgram.SetUniform1i("u_DebugMode", 0);
		}
	}
}
//
//MainRenderer::~MainRenderer()
//{
//	std::cout << "[Warning] : Main Renderer destroied\n";
//}

void MainRenderer::CreateLightDebugObj()
{
	static CubeMesh cube_mesh;
	cube_mesh.Create();
	static Texture texture("Assets/Textures/plain64.png");
	m_CubeDebugObject = new GameObject(&cube_mesh, &texture);
	m_CubeDebugObject->worldScale = glm::vec3(0.25f);
}

void MainRenderer::ProcessLight()
{
	if (m_PointLights.size() > 0)
		m_MainShaderProgram.UsePointLight(m_PointLights, m_PointLights.size()/*(sizeof(m_PointLight) / sizeof(m_PointLight[0]))*/);

	if (m_SpotLights.size() > 0)
		m_MainShaderProgram.UseSpotLight(m_SpotLights, m_SpotLights.size());

	if (m_DirectionLights.size() > 0)
		m_MainShaderProgram.UseDirectionalLight(m_DirectionLights, m_DirectionLights.size());

}
