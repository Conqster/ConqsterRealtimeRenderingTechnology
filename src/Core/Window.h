#pragma once
#include "glm/glm.hpp"

struct WindowProperties
{
	const char* title;
	uint32_t width;
	uint32_t height;

	WindowProperties(const char* title = "Default Title",
					uint32_t width = 2560, uint32_t height = 1440)
		: title(title), width(width), height(height){}
};



struct GLFWwindow;

class Window
{
public:
	Window();
	Window(uint32_t width, uint32_t height);
	Window(uint32_t width, uint32_t height, const char* program_name);

	Window(const WindowProperties& window_prop);


	Window* Create(const WindowProperties& window_prop);

	bool Init();

	void ClearScreen(glm::vec3 colour) const;
	void SwapBuffers() const;
	void ToggleLockCursor();
	bool WindowShouldClose() const;

	void OnUpdate(bool full = true);
	void OnWaitUpdate();

	void Close() const;

	inline bool const Initilised() { return m_Initilised; }
	inline unsigned int const GetWidth() { return m_InitProp.width; }
	inline unsigned int const GetHeight() { return m_InitProp.height; }
	inline float const GetAspectRatio() { return (float)m_InitProp.width / (float)m_InitProp.height; }
	glm::vec2 GetMouseScreenPosition();

	inline bool* const Ptr_LockCursorFlag() { return &m_LockCursor; }

	GLFWwindow* GetNativeWindow() const { return m_Window; }

	WindowProperties* const GetInitProp() { return &m_InitProp; }

	void UpdateProgramTitle(const char* title);

	~Window();
private:
	static int m_Width;
	static int m_Height;

	WindowProperties m_InitProp;

	GLFWwindow* m_Window = nullptr;
	const char* m_ProgramName = "default";

	static void HandleWindowResizeCallback(GLFWwindow* window, int width, int height);
	static bool m_OnResize;

	bool m_LockCursor = false;
	bool m_Initilised = false;
};
