#include "EventHandle.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <cstring>

#include "glm/glm.hpp"

#include "Util/GameTime.h"

bool EventHandle::keys[NUM_KEYS];
bool EventHandle::mouseButton[NUM_BUTTONS][NUM_ACTIONS];
bool EventHandle::mouseButtonEventCount;

float EventHandle::lastX;
float EventHandle::lastY;
float EventHandle::xChange;
float EventHandle::yChange;
bool EventHandle::mouseFirstMoved;
glm::vec2 EventHandle::m_CurrentMousePos;


void EventHandle::CreateCallBacks(GLFWwindow* window)
{

	glfwSetKeyCallback(window, HandleKeys);
	glfwSetCursorPosCallback(window, HandleMouse);

	glfwSetMouseButtonCallback(window, HandleMouseButton);

	mouseButtonEventCount = sizeof(mouseButton) / sizeof(mouseButton[0][0]);

	std::cout << "Created callbacks......\n";
}

int EventHandle::GetKeyState(GLFWwindow* window, int key)
{
	return glfwGetKey(window, key);
}


float EventHandle::MouseXChange()
{
	float dt = xChange;
	xChange = 0.0f;
	return dt;
}

float EventHandle::MouseYChange()
{
	float dt = yChange;
	yChange = 0.0f;
	return dt;
}

glm::vec2 EventHandle::MousePosition()
{
	return m_CurrentMousePos;
}

void EventHandle::PollEvents()
{
	//TimeTaken poll_event_time("Polling Event Time taken");

	memset(mouseButton, 0, sizeof(mouseButton));
	glfwPollEvents();
}

void EventHandle::WaitPollEvents()
{
	memset(mouseButton, 0, sizeof(mouseButton));
	glfwWaitEvents();
}


void EventHandle::HandleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{

	//To-Do (Fix): a data struture that allow notification of which input and corresponding action
	//std::cout << " MAIN Called .........\n";
	if (key >= 0 && key < NUM_KEYS && action >= 0 && action < NUM_ACTIONS)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
			//std::cout << "pressed: " << key << "\n";
		}
		if (action == GLFW_RELEASE)
		{
			keys[key] = false;
			//std::cout << "released: " << key << "\n";
		}
	}
}

void EventHandle::HandleMouse(GLFWwindow* window, double xPos, double yPos)
{
	if (mouseFirstMoved)
	{
		lastX = xPos;
		lastY = yPos;
		mouseFirstMoved = false;
	}

	xChange = xPos - lastX;
	yChange = lastY - yPos;

	xChange += xPos - lastX;
	yChange += lastY - yPos;

	lastX = xPos;
	lastY = yPos;


	m_CurrentMousePos = glm::vec2(xPos, yPos);

}

void EventHandle::HandleMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (button >= 0 && button < NUM_BUTTONS && action >= 0 && action < NUM_ACTIONS)
	{
		mouseButton[button][action] = true;
	}


}

