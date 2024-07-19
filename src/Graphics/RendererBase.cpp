#include "RendererBase.h"

#include "../Core/Window.h"
#include <iostream>

RendererBase::RendererBase(unsigned int width, unsigned int height)
{
	m_Window = new Window(width, height);
}

RendererBase::RendererBase(Window& use_window)
{
	//TO-DO: might want o change this 
	//if i have other renderer using window
	if (m_Window)
		delete m_Window;
	else
		m_Window = &use_window;
}

bool RendererBase::Init()
{
	if (!m_Window)
		return false;

	if (m_Window->Initilised())
		return true;

	return m_Window->Init();
}

void RendererBase::CreateShader(const ShaderFilePath& shader_file, const glm::mat4& viewProj, bool setup_lights)
{
	m_MainShaderProgram.CreateFromFile(shader_file.vertexPath, shader_file.fragmentPath);
	if(setup_lights)
		m_MainShaderProgram.SetupLights();

	m_MainShaderProgram.UseShader();

	m_MainShaderProgram.SetUniformMat4f("u_projection", viewProj);
}

void RendererBase::SetShaderViewProjection(const glm::mat4& viewProj)
{
	//TO-DO: for now just the main shader, later could apply to multiple
	m_MainShaderProgram.UseShader();

	m_MainShaderProgram.SetUniformMat4f("u_projection", viewProj);
}

void RendererBase::ClearScreen() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererBase::RenderObjects(Camera& camera)
{

}

void RendererBase::SwapOpenGLBuffers() const
{
	m_Window->SwapBuffers();
}

bool RendererBase::WindowShouldClose()
{
	return m_Window->WindowShouldClose();
}

void RendererBase::CloseWindow()
{
	if (m_Window)
	{
		m_Window->Close();
		delete m_Window;
		m_Window = nullptr;
	}
}

RendererBase::~RendererBase()
{
	std::cout << "[Warning] : Renderer Base has closed, so as GLFW!!!!!\n";
}
