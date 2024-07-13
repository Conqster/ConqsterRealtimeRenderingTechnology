#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

#include "GameObject.h"
#include "Graphics/Shader.h"

#include "Graphics/Lights/SpotLight.h"
#include "Graphics/Lights/DirectionalLight.h"

#include <vector>
#include <memory>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* func, const char* file, int line);


class Renderer
{
public:
	Renderer();
	Renderer(int width, int height);

	bool Init();

	void CreateGlobalShaderFromFile(const std::string& vert_file_path, const std::string& frag_file_path);
	//void CreateGlobalShader(const char* vertex = globalVertexShader, const char* frag = globalFragmentShader);
	void CreateProjectionViewMatrix();
	
	void ClearScreen() const;
	void RenderObjects(const std::vector<GameObject*> objects, glm::mat4& camera_view_mat);
	void SwapOpenGLBuffers() const;

	inline struct GLFWwindow* GetWindow() { return m_Window; }
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

	inline bool* const LockCursorFlag() { return &m_lockCursor; }
	void ToggleLockCursor();

	//inline Light* const GetLight() 
	//{ if(m_PointLights.size() > 0) return m_Lights[0]; }

	inline std::vector<Light*>& const GetLights() { return m_Lights; }

	inline unsigned int const LightsCount() { return m_Lights.size(); }

	bool WindowShouldClose();
	void CloseWindow();

private:
	int m_Width;
	int	m_Height;

	struct GLFWwindow* m_Window = nullptr;

	Shader m_GlobalShaderProgram;
	GameObject* m_CubeDebugObject;
	//PointLight m_PointLight[Shader_Constants::MAX_POINT_LIGHTS];
	std::vector<std::unique_ptr<PointLight>> m_PointLights;
	std::vector<std::unique_ptr<SpotLight>> m_SpotLights;
	std::vector<std::unique_ptr<DirectionalLight>> m_DirectionLights;
	std::vector<Light*> m_Lights;
	void AddPointLight(std::unique_ptr<PointLight> light);
	void AddSpotLight(std::unique_ptr<SpotLight> light);
	void AddDirectionalLight(std::unique_ptr<DirectionalLight> light);
	glm::vec4 m_AmbientLightColour = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	glm::vec3 m_TestLightPos = glm::vec3();
	float m_AmbientLightStrength = 0.1f;

	bool m_lockCursor = false;

	unsigned int m_LineVAO;
	unsigned int m_LineVBO;


	void CreateLine();
};

