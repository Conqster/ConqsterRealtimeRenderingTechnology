#pragma once

#include "glm/glm.hpp"
#include "Shader.h"
#include "ObjectBuffer/UniformBuffer.h"
#include "Meshes/SphereMesh.h"

#include <memory>

class DebugGizmos
{
public:
	DebugGizmos() = default;
	~DebugGizmos();

	static void Startup();
	static void DrawLine(glm::vec3 v1, glm::vec3 v2, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawWireSphere(glm::vec3 p, float radius = 1.0f, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawSphere(glm::vec3 p, float radius = 1.0f, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f));
	static void Cleanup();

private:
	static bool active;
	static Shader m_Shader;
	static SphereMesh sphere;

	static void Generate();
	//make it a pointer for now to update it extrenally 
	//static std::shared_ptr<UniformBuffer> m_CameraMatUBO;
};