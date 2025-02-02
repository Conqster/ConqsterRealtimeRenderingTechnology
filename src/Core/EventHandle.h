#pragma once
#include "glm/glm.hpp"

constexpr int NUM_BUTTONS = 7;
constexpr int NUM_ACTIONS = 3;
constexpr int NUM_KEYS = 1024;

class EventHandle
{
public:
	static void CreateCallBacks(struct GLFWwindow* window);

	inline static bool* GetKeys() { return keys; }
	static int GetKeyState(GLFWwindow* window, int key);

	static bool (*GetMouseButton())[3] { return mouseButton; }

	static float MouseXChange();
	static float MouseYChange();

	static glm::vec2 MousePosition();

	static void PollEvents();
	static void WaitPollEvents();

	static void Flush();
	
private: 
	static void HandleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void HandleMouse(GLFWwindow* window, double xPos, double yPos);
	static void HandleMouseButton(GLFWwindow* window, int button, int action, int mods);
	
	static bool keys[NUM_KEYS];
	static bool mouseButton[NUM_BUTTONS][NUM_ACTIONS];
	static bool mouseButtonEventCount;

	static float lastX;
	static float lastY;
	static float xChange;
	static float yChange;
	static bool mouseFirstMoved;

	static glm::vec2 m_CurrentMousePos;
};

