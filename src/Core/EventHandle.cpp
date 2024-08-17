#include "EventHandle.h"
#include "GLFW/glfw3.h"
#include <iostream>

#include "glm/glm.hpp"

bool EventHandle::keys[1024];

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

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	std::cout << "Created callbacks......\n";
}

bool* EventHandle::GetKeys()
{
	return keys;
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


void EventHandle::HandleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	//std::cout << " MAIN Called .........\n";
	if (key >= 0 && key < 1024)
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

	//xChange += xPos - lastX;
	//yChange += lastY - yPos;

	lastX = xPos;
	lastY = yPos;


	m_CurrentMousePos = glm::vec2(xPos, yPos);

}