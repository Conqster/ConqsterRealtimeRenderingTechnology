#include "Window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "EventHandle.h"

Window::Window() 
	: m_Width(1024), m_Height(768), m_LockCursor(false)
{}

Window::Window(unsigned int width, unsigned int height)
	: m_Width(width), m_Height(height), m_LockCursor(false)
{}

Window::Window(unsigned int width, unsigned int height, const char* program_name)
	: m_Width(width), m_Height(height), m_ProgramName(program_name), m_LockCursor(false)
{}

bool Window::Init()
{
	if (!glfwInit())
	{
		std::cout << "[GLFW ERROR]: Failed to initalise GLFW!!!!!!!!\n";
		glfwTerminate();
		return false;
	}

	//setup GLFW window propreties 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	m_Window = glfwCreateWindow(m_Width, m_Height, m_ProgramName, NULL, NULL);

	if (!m_Window)
	{
		std::cout << "[GLFW ERROR]: Window mode creation and its OpenGL context failed!!!!!!!\n";
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_Window);
	std::cout << "[GRAPHICS INFO]: openGL Version: " << glGetString(GL_VERSION) << "\n";

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
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	//Setup Viewport size
	glViewport(0, 0, m_Width, m_Height);

	//glfwSetWindowUserPointer(m_Window, this);
	float center_x = m_Width * 0.5f;
	float center_y = m_Height * 0.5f;


	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPos(m_Window, center_x, center_y);
	glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);
	m_LockCursor = true;

	m_Initilised = true;
	return true;
}

void Window::SwapBuffers() const
{
	glfwSwapBuffers(m_Window);
}

void Window::ToggleLockCursor()
{
	m_LockCursor = !m_LockCursor;
	unsigned int cursor_state = (m_LockCursor) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
	glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);
	//float center_x = m_Width * 0.5f;
	//float center_y = m_Height * 0.5f;
	//glfwSetCursorPos(m_Window, center_x, center_y);
	glfwSetInputMode(m_Window, GLFW_CURSOR, cursor_state);
}

bool Window::WindowShouldClose() const
{
	return glfwWindowShouldClose(m_Window);
}

void Window::Close() const
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
	std::cout << "[Warning] : Window has closed, so as GLFW!!!!!\n";
}

glm::vec2 Window::GetMouseScreenPosition()
{
	double x, y;
	glfwGetCursorPos(m_Window, &x, &y);
	return glm::vec2(x, y);
}

Window::~Window()
{
	std::cout << "[Warning] : Window destructor called\n";
}


