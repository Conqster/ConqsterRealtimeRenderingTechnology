#pragma once

#include "Shader.h"
#include "../Core/Window.h"

class Window;

class RendererBase
{

public:
	RendererBase() = default;
	RendererBase(unsigned int width, unsigned int height);
	RendererBase(Window& use_window);

	bool Init();

	void CreateShader(const ShaderFilePath& shader_file, const glm::mat4& proj, bool swtup_lights);
	void SetShaderViewProjection(const glm::mat4& viewProj);

	void ClearScreen() const;
	void RenderObjects(class Camera& camera);
	void SwapOpenGLBuffers() const;

	inline const char* GetName() { return m_SceneName; }
	inline Window* const GetWindow() { return m_Window; }
	inline const Shader* GetCurrentMainShaderProgram() const { return &m_MainShaderProgram; }

	bool WindowShouldClose();

	virtual void CloseWindow();


	void ToggleLockCursor()
	{
		m_Window->ToggleLockCursor();
	}

	~RendererBase();
protected:
	const char* m_SceneName = "None";
	Window* m_Window = nullptr;

	Shader m_MainShaderProgram;
};