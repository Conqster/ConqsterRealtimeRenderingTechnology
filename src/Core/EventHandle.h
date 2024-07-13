#pragma once
#include "glm/glm.hpp"

class EventHandle
{
public:
	static void CreateCallBacks(struct GLFWwindow* window);

	static bool* GetKeys();
	static int GetKeyState(GLFWwindow* window, int key);

	static float MouseXChange();
	static float MouseYChange();

	static glm::vec2 MousePosition();
	
private: 
	static void HandleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void HandleMouse(GLFWwindow* window, double xPos, double yPos);
	static bool keys[1024];


	static float lastX;
	static float lastY;
	static float xChange;
	static float yChange;
	static bool mouseFirstMoved;

	static glm::vec2 m_CurrentMousePos;
};

