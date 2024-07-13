#pragma once
#include "Renderer.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "EventHandle.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Util/Vector.h"

#include "Meshes/CubeMesh.h"
#include "Texture.h"

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



Renderer::Renderer() : m_Window(nullptr), m_lockCursor(false)
{
	m_Width = 800;
	m_Height = 600;

	m_Width = 1920;
	m_Height = 1080;

	//m_Width = 960;
	//m_Height = 540;
}

Renderer::Renderer(int width, int height) : m_Width(width), 
				  m_Height(height), m_Window(nullptr), m_lockCursor(false)
{
}


bool Renderer::Init()
{

	if (!glfwInit())
	{
		std::cout << "GLFW init Failed.......\n";
		glfwTerminate();
		return false;
	}



	//setup GLFW window propreties 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	m_Window = glfwCreateWindow(m_Width, m_Height, "Testing Renderer", NULL, NULL);


	if (!m_Window)
	{
		
		std::cout << "Window mode creation and its OpenGL context failed!!!!!!!\n";
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_Window);
	std::cout << "Init Window...........\n";
	std::cout << "GRAPHICS INFO: openGL Version: " << glGetString(GL_VERSION) << "\n";

	glfwSwapInterval(1);

	EventHandle::CreateCallBacks(m_Window);

	//allow modern extension features
	glewExperimental = GL_TRUE;

	GLenum GlewInitResult = glewInit();

	if (GlewInitResult != GLEW_OK)
	{
		std::cout << "Glew Init failed, ERROR: " << glewGetErrorString(GlewInitResult) << "\n";
		glfwDestroyWindow(m_Window);
		glfwTerminate();
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	//Setup Viewport size
	glViewport(0, 0, m_Width, m_Height);

	//glfwSetWindowUserPointer(m_Window, this);
	float center_x = m_Width * 0.5f;
	float center_y = m_Height * 0.5f;

	glfwSetCursorPos(m_Window, center_x, center_y);

	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_lockCursor = true;

	return true;
}



void Renderer::CreateGlobalShaderFromFile(const std::string& vert_file_path, const std::string& frag_file_path)
{
	m_GlobalShaderProgram.CreateFromFile(vert_file_path, frag_file_path);
	m_GlobalShaderProgram.UseShader();
	m_GlobalShaderProgram.SetUniform1i("u_Texture", 0);

	//m_PointLight[0] = PointLight(1.0f, 0.0f, 0.0f, 0.5f, 0.6f, glm::vec3());
	AddDirectionalLight(std::make_unique<DirectionalLight>());
	AddPointLight(std::make_unique<PointLight>(0.0f, 1.0f, 0.0f, 0.5f, 0.6f, glm::vec3(-3.0f, 4.0f, -4.0f)));
	AddSpotLight(std::make_unique<SpotLight>(0.0f, 0.0f, 1.0f, 0.5f, 0.6f, glm::vec3(3.0f, 3.0f, 0.0f)));
	AddSpotLight(std::make_unique<SpotLight>(1.0f, 1.0f, 1.0f, 0.5f, 0.6f, glm::vec3(0.0f, 15.0f, -10.0f)));
	AddSpotLight(std::make_unique<SpotLight>(1.0f, 1.0f, 1.0f, 0.5f, 0.6f, glm::vec3(0.0f, 15.0f, -10.0f)));
	AddPointLight(std::make_unique<PointLight>(1.0f, 0.0f, 0.0f, 0.5f, 0.6f, glm::vec3(5.0f, 10.0f, 0.0f)));
	AddPointLight(std::make_unique<PointLight>(1.0f, 0.0f, 0.0f, 0.5f, 0.6f, glm::vec3(10.0f, 10.0f, 10.0f)));


	//m_PointLight[2] = PointLight(0.0f, 1.0f, 0.0f, 0.5f, 0.6f, glm::vec3());
	static CubeMesh cube_mesh;
	cube_mesh.Create();
	static Texture texture("Assets/Textures/plain64.png");
	m_CubeDebugObject = new GameObject(&cube_mesh, &texture);
	//m_CubeDebugObject->worldPos = glm::vec3(m_PointLights[0].GetPosition());
	m_CubeDebugObject->worldScale = glm::vec3(0.25f);

}

//void Renderer::CreateGlobalShader(const char* vertex, const char* frag)
//{
//	//triangle shader
//	m_GlobalShaderProgram.CreateFromCode(vertex, frag);
//
//	m_GlobalShaderProgram.UseShader();
//	//0 - because the bind active texture is at slot 0 == glActiveTexture(GL_TEXTURE0)
//	//maybe i could have multiple  slots in one texture
//	m_GlobalShaderProgram.SetUniform1i("u_Texture", 0);
//
//
//	//CREATE LINE VAO VBO propreties
//	CreateLine();
//}

void Renderer::CreateProjectionViewMatrix()
{
	m_GlobalShaderProgram.UseShader();

	//								  l_egde|r_egde|t_edge|b_egde | near  | far plane
	//glm::mat4 projection = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
	//glm::mat4 projection = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);   //Previous
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)m_Width / m_Height, 0.1f, 150.0f/*10000.0f*/);

	m_GlobalShaderProgram.SetUniformMat4f("u_projection", projection/*glm::mat4(1.0f)*/);

}



void Renderer::ClearScreen() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SwapOpenGLBuffers() const
{
	GLCall(glfwSwapBuffers(m_Window));
}


void Renderer::RenderObjects(const std::vector<GameObject*> objects, glm::mat4& camera_view_mat)
{
	m_GlobalShaderProgram.UseShader();
	m_GlobalShaderProgram.SetUniformMat4f("u_view", camera_view_mat);


	if(m_PointLights.size() > 0)
		m_GlobalShaderProgram.UsePointLight(m_PointLights, m_PointLights.size()/*(sizeof(m_PointLight) / sizeof(m_PointLight[0]))*/);

	if (m_SpotLights.size() > 0)
		m_GlobalShaderProgram.UseSpotLight(m_SpotLights, m_SpotLights.size());

	if (m_DirectionLights.size() > 0)
		m_GlobalShaderProgram.UseDirectionalLight(m_DirectionLights, m_DirectionLights.size());

	//DRAW GRID
	if (false)
	{
		GLCall(glBindVertexArray(m_LineVAO));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO));
		m_GlobalShaderProgram.SetUniform1i("u_DebugMode", 1);


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
			m_GlobalShaderProgram.SetUniformMat4f("u_model", line_model);
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
			m_GlobalShaderProgram.SetUniformMat4f("u_model", line_model);
			GLCall(glDrawArrays(GL_LINES, 0, 2));
		}


		m_GlobalShaderProgram.SetUniform1i("u_DebugMode", 0);
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	}
	

	static float colour_b = 0.1f;
	static int incrementDir = 1;

	colour_b += incrementDir * 0.01f;
	if (colour_b >= 1)
		incrementDir = -1;
	else if (colour_b <= 0)
		incrementDir = 1;

	m_GlobalShaderProgram.SetUniform4f("u_TestColour", -colour_b, 0.0f, colour_b, 1.0f);

	glm::mat4 gameobject_model;
	for (size_t i = 0; i < objects.size(); i++)
	{
		auto& gameobject = objects[i];
		gameobject_model = glm::mat4(1.0f);
		gameobject_model = glm::translate(gameobject_model, gameobject->worldPos);
		//first 3 == xyz
		gameobject_model = glm::rotate(gameobject_model, glm::radians(gameobject->rotation.w), (glm::vec3)gameobject->rotation);
		gameobject_model = glm::scale(gameobject_model,/* 50.0f **/ gameobject->worldScale);

		m_GlobalShaderProgram.SetUniformMat4f("u_model", gameobject_model);
		m_GlobalShaderProgram.UseShader();

		bool debug_mode = false;
		//DEBUGING GAMEOBJECT
		if (gameobject->selected)
		{
			m_GlobalShaderProgram.SetUniform1i("u_DebugMode", 1);
			//gameobject->Draw(); //solid debug
			gameobject->DebugDraw(); //line debug
			m_GlobalShaderProgram.SetUniform1i("u_DebugMode", 0);
		}
		else if(debug_mode)
		{
			//gameobject->Draw();
			m_GlobalShaderProgram.SetUniform1i("u_DebugMode", 1);
			//gameobject->Draw(); //solid debug
			gameobject->DebugDraw(); //line debug
			m_GlobalShaderProgram.SetUniform1i("u_DebugMode", 0);
		}

		gameobject->Draw();
	}

	//////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////POINT LIGHTS DEBUG LOCATION////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	
	glm::mat4 model(1.0f);
	for (auto& light: m_Lights)
	{
		if(auto a_point_light = dynamic_cast<PointLight*>(light))
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, a_point_light->GetPosition());
			model = glm::scale(model, m_CubeDebugObject->worldScale);
			m_GlobalShaderProgram.UseShader();
			m_GlobalShaderProgram.SetUniformMat4f("u_model", model);
			m_CubeDebugObject->Draw();
			//m_GlobalShaderProgram.SetUniform1i("u_DebugMode", 1);
			//m_CubeDebugObject->DebugDraw();
			//m_GlobalShaderProgram.SetUniform1i("u_DebugMode", 0);
		}
	}


}



void Renderer::ToggleLockCursor()
{
	m_lockCursor = !m_lockCursor;
	unsigned int cursor_state = (m_lockCursor) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
	glfwSetInputMode(m_Window, GLFW_CURSOR, cursor_state);
}




bool Renderer::WindowShouldClose()
{
	GLCall(return glfwWindowShouldClose(m_Window));
}

void Renderer::CloseWindow()
{
	m_CubeDebugObject->Cleanup();
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}


void Renderer::AddPointLight(std::unique_ptr<PointLight> light)
{
	if (m_PointLights.size() > Shader_Constants::MAX_POINT_LIGHTS)
		return;

	m_Lights.push_back(light.get());
	m_PointLights.push_back(std::move(light));
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

void Renderer::AddSpotLight(std::unique_ptr<SpotLight> light)
{
	if (m_PointLights.size() > Shader_Constants::MAX_SPOT_LIGHTS)
		return;

	m_Lights.push_back(light.get());
	m_SpotLights.push_back(std::move(light));
}

void Renderer::AddDirectionalLight(std::unique_ptr<DirectionalLight> light)
{
	m_Lights.push_back(light.get());
	m_DirectionLights.push_back(std::move(light));
	//m_Lights.push_back(light);
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



