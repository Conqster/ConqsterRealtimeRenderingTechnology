#pragma once
#include "Renderer.h"

#include "../Core/Window.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include "EventHandle.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Util/Vector.h"

#include "Meshes/CubeMesh.h"
#include "Texture.h"

#include "Camera.h"

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR)
	{
		GLenum ErrorCheckValue = glGetError();
		std::cout << "stuck in a while loop Renderer.cpp: " << glewGetErrorString(ErrorCheckValue) << "\n";
	}
}

bool GLLogCall(const char* func, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] -> " << error <<
			",  " << func << " " << file << ":" << line << "\n";
		return false;
	}
	return true;
}



Renderer::Renderer()
{
	m_Window = new Window();
}

Renderer::Renderer(unsigned int width, unsigned int height)
{
	m_Window = new Window(width, height);
}


bool Renderer::Init()
{
	return m_Window->Init();
}

void Renderer::CreateMainShader(const ShaderFilePath& shader_file, const glm::mat4& viewProj)
{
	m_MainShaderProgram.CreateFromFile(shader_file.vertexPath, shader_file.fragmentPath);
	m_MainShaderProgram.UseShader();

	m_MainShaderProgram.SetUniformMat4f("u_projection", viewProj);
}


void Renderer::UpdateShaderViewProjection(const glm::mat4& viewProj)
{
	//TO-DO: for now just the main shader, later could apply to multiple
	m_MainShaderProgram.UseShader();

	m_MainShaderProgram.SetUniformMat4f("u_projection", viewProj);
}



void Renderer::ClearScreen() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SwapOpenGLBuffers() const
{
	m_Window->SwapBuffers();
}


void Renderer::RenderObjects(const std::vector<GameObject*> objects, Camera& camera)
{
	m_MainShaderProgram.UseShader();
	m_MainShaderProgram.SetUniformMat4f("u_view", camera.CalViewMat());
	m_MainShaderProgram.SetUniformVec3("u_ViewPos", camera.GetPosition());


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
		else if(debug_mode)
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
		RenderGrid();
	}


	//////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////POINT LIGHTS DEBUG LOCATION////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	
	glm::mat4 model(1.0f);
	for (auto& light: m_Lights)
	{
		if(auto a_point_light = dynamic_cast<PointLight*>(light))
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



void Renderer::ToggleLockCursor()
{
	m_Window->ToggleLockCursor();
}




bool Renderer::WindowShouldClose()
{
	return m_Window->WindowShouldClose();
}

void Renderer::CloseWindow()
{
	delete m_CubeDebugObject;
	m_CubeDebugObject = nullptr;

	m_PointLights.clear();
	m_SpotLights.clear();
	m_DirectionLights.clear();

	m_Lights.clear();

	

	if (m_Window)
	{
		m_Window->Close();
		delete m_Window;
		m_Window = nullptr;
	}
}


void Renderer::AddPointLight(std::unique_ptr<PointLight> point_light)
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

void Renderer::AddSpotLight(std::unique_ptr<SpotLight> spot_light)
{
	if (m_PointLights.size() > Shader_Constants::MAX_SPOT_LIGHTS)
		return;

	m_Lights.push_back(spot_light.get());
	m_SpotLights.push_back(std::move(spot_light));
}

void Renderer::AddDirectionalLight(std::unique_ptr<DirectionalLight> directional_light)
{
	m_Lights.push_back(directional_light.get());
	m_DirectionLights.push_back(std::move(directional_light));
	//m_Lights.push_back(light);
}

void Renderer::CreateLightDebugObj()
{
	static CubeMesh cube_mesh;
	cube_mesh.Create();
	static Texture texture("Assets/Textures/plain64.png");
	m_CubeDebugObject = new GameObject(&cube_mesh, &texture);
	m_CubeDebugObject->worldScale = glm::vec3(0.25f);
}

void Renderer::ProcessLight()
{
	if (m_PointLights.size() > 0)
		m_MainShaderProgram.UsePointLight(m_PointLights, m_PointLights.size()/*(sizeof(m_PointLight) / sizeof(m_PointLight[0]))*/);

	if (m_SpotLights.size() > 0)
		m_MainShaderProgram.UseSpotLight(m_SpotLights, m_SpotLights.size());

	if (m_DirectionLights.size() > 0)
		m_MainShaderProgram.UseDirectionalLight(m_DirectionLights, m_DirectionLights.size());

}

void Renderer::CreateLine()
{
	glGenVertexArrays(1, &m_LineVAO);
	glBindVertexArray(m_LineVAO);

	float vertices[] =
	{
	//	 x      y    z      w        r     g    b    a
		1.0f, 0.0f, 0.0f,  1.0f,      0.43f, 0.43f, 0.43f, 1.0f,
		-1.0f, 0.0f, 0.0f, 1.0f,    0.43f, 0.43f, 0.43f, 1.0f,
		//1.0f, -1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,
	};

	GLCall(glGenBuffers(1, &m_LineVBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	//pos location in shader (location = 0)
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0));
	//colour location in shader (location = 1)
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 4)));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
}

void Renderer::RenderGrid()
{
	GLCall(glBindVertexArray(m_LineVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO));
	m_MainShaderProgram.SetUniform1i("u_DebugMode", 1);


	unsigned int length = 500;
	unsigned int half_length = length * 0.5f;
	glm::mat4 line_model = glm::mat4(1.0f);

	//LENGTH
	for (unsigned int i = 0; i < length; i++)
	{
		int offset = half_length - i;
		line_model = glm::mat4(1.0f);
		line_model = glm::translate(line_model, glm::vec3(0.0f, 0.0f, offset));
		//line_model = glm::rotate
		line_model = glm::scale(line_model, glm::vec3(half_length));
		m_MainShaderProgram.SetUniformMat4f("u_model", line_model);
		GLCall(glDrawArrays(GL_LINES, 0, 2));
	}

	//BREATH
	for (unsigned int i = 0; i < length; i++)
	{
		int offset = half_length - i;
		line_model = glm::mat4(1.0f);
		line_model = glm::translate(line_model, glm::vec3(offset, 0.0f, 0.0f));
		line_model = glm::rotate(line_model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		line_model = glm::scale(line_model, glm::vec3(half_length));
		m_MainShaderProgram.SetUniformMat4f("u_model", line_model);
		GLCall(glDrawArrays(GL_LINES, 0, 2));
	}


	m_MainShaderProgram.SetUniform1i("u_DebugMode", 0);
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}



