#include "Window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "EventHandle.h"

#include <string>


int Window::m_Width;
int Window::m_Height;
bool Window::m_OnResize;

Window::Window() 
	:  m_LockCursor(false)
{}

Window::Window(unsigned int width, unsigned int height)
	: m_LockCursor(false)
{}

Window::Window(unsigned int width, unsigned int height, const char* program_name)
	: m_ProgramName(program_name), m_LockCursor(false)
{}

Window::Window(const WindowProperties& window_prop)
	: m_LockCursor(false)
{
	m_Width = window_prop.width;
	m_Height = window_prop.height;
	m_ProgramName = window_prop.title;

	m_InitProp = window_prop;

	Init();
}

Window* Window::Create(const WindowProperties& window_prop)
{
	m_Width = window_prop.width;
	m_Height = window_prop.height;
	m_ProgramName = window_prop.title;

	m_InitProp = window_prop;

	Init();
	return this;
}

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

	m_Window = glfwCreateWindow(m_InitProp.width, m_InitProp.height, m_ProgramName, NULL, NULL);

	if (!m_Window)
	{
		std::cout << "[GLFW ERROR]: Window mode creation and its OpenGL context failed!!!!!!!\n";
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_Window);
	std::cout << "[GRAPHICS INFO]: openGL Version: " << glGetString(GL_VERSION) << "\n";

	glfwSwapInterval(1);

	//Set glfw callbacks  
	EventHandle::CreateCallBacks(m_Window);

	//To-do: a quick fix to assign handle for window size call back
	// had to make it static
	//glfwSetWindowSizeCallback(m_Window, HandleWindowResizeCallback);
	glfwSetFramebufferSizeCallback(m_Window, HandleWindowResizeCallback);

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


	//glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPos(m_Window, center_x, center_y);
	glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);
	//m_LockCursor = true;

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

void Window::UpdateProgramTitle(const char* title)
{
	m_ProgramName =
		m_InitProp.title = title;

	glfwSetWindowTitle(m_Window, m_ProgramName);
}


void Window::HandleWindowResizeCallback(GLFWwindow* window, int width, int height)
{
	m_Width = width;
	m_Height = height;

	glViewport(0, 0, m_Width, m_Height);

	m_OnResize = true;
}


glm::vec2 Window::GetMouseScreenPosition()
{
	double x, y;
	glfwGetCursorPos(m_Window, &x, &y);
	return glm::vec2(x, y);
}

void Window::OnUpdate()
{
	if (m_OnResize)
	{
		m_InitProp.width = m_Width;
		m_InitProp.height = m_Height;
		
		m_OnResize = false;
		std::cout << "Window Resized!!!!!!!!!\n";
	}

	EventHandle::PollEvents();
	glfwSwapBuffers(m_Window);
}

Window::~Window()
{
	std::cout << "[Warning] : Window destructor called\n";
}


