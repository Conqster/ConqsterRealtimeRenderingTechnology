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


class Window;

class Renderer
{
public:
	Renderer();
	Renderer(unsigned int width, unsigned int height);

	bool Init();

	void CreateMainShader(const ShaderFilePath& shader_file, const glm::mat4& viewProj);
	void UpdateShaderViewProjection(const glm::mat4& viewProj);
	
	void ClearScreen() const;
	void RenderObjects(const std::vector<GameObject*> objects,class Camera& camera);
	void SwapOpenGLBuffers() const;

	void AddPointLight(std::unique_ptr<PointLight> point_light);
	void AddSpotLight(std::unique_ptr<SpotLight> spot_light);
	void AddDirectionalLight(std::unique_ptr<DirectionalLight> directional_light);


	void ToggleLockCursor();

	inline Window* const GetWindow() { return m_Window; }

	inline std::vector<Light*>& const GetLights() { return m_Lights; }
	inline const Shader* GetCurrentMainShaderProgram() const { return &m_MainShaderProgram; }

	inline unsigned int const LightsCount() { return m_Lights.size(); }

	bool WindowShouldClose();
	void CloseWindow();

private:
	Window* m_Window = nullptr;

	Shader m_MainShaderProgram;
	GameObject* m_CubeDebugObject = nullptr;
	//PointLight m_PointLight[Shader_Constants::MAX_POINT_LIGHTS];
	std::vector<std::unique_ptr<PointLight>> m_PointLights;
	std::vector<std::unique_ptr<SpotLight>> m_SpotLights;
	std::vector<std::unique_ptr<DirectionalLight>> m_DirectionLights;
	std::vector<Light*> m_Lights;

	void CreateLightDebugObj();

	void ProcessLight();


	//TO-DO: Need to change this later
	unsigned int m_LineVAO;
	unsigned int m_LineVBO;

	void CreateLine();
	void RenderGrid();
};

