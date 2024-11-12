#pragma once

#include "glm/glm.hpp"
#include "Shader.h"
#include "ObjectBuffer/UniformBuffer.h"
#include "Meshes/SphereMesh.h"

#include "SceneRenderer.h"

#include <memory>

#include "Geometry/AABB.h"
#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"

class DebugGizmos
{
public:
	DebugGizmos() = default;
	~DebugGizmos();

	static void Startup();
	static void DrawLine(glm::vec3 v1, glm::vec3 v2, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawRay(glm::vec3 v1, glm::vec3 dir, float dist = 100.0f, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawWireSphere(glm::vec3 p, float radius = 1.0f, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawSphere(glm::vec3 p, float radius = 1.0f, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f));
	static void DrawSquare(glm::vec3 center, glm::vec3 forward, float left, float right, float bottom, float top, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawBox(AABB aabb, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawCross(glm::vec3 center, float size = 1.0f, bool axis_colour = true, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawWireDisc(glm::vec3 center, glm::vec3 right, glm::vec3 up, float radius, int steps = 5, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawWireThreeDisc(glm::vec3 center, float radius, int steps = 5, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawWireCone(glm::vec3 center, glm::vec3 up, glm::vec3 right, float radius = 1.0f, float height = 2.0f, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawOrthoCameraFrustrm(glm::vec3 pos, glm::vec3 forward, float cam_near, float cam_far, float size, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawPerspectiveCameraFrustum(glm::vec3 pos, glm::vec3 forward, float fov, float aspect, float cam_near, float cam_far, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawPerspectiveCameraFrustum(glm::vec3 pos, glm::vec3 forward, glm::vec3 up, float fov, float aspect, float cam_near, float cam_far, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawPlane(const Plane& f, glm::vec2 size = glm::vec2(10.0f, 10.0f), glm::vec3 col = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawWirePlane(const Plane& f, glm::vec2 size = glm::vec2(10.0f, 10.0f), glm::vec3 col = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawFrustum(const Frustum& f, glm::vec3 col = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void Cleanup();

private:
	static bool active;
	static bool use;
	static Shader m_Shader;
	static SphereMesh sphere;

	static CRRT::SceneRenderer m_Renderer;

	static void Generate();
	//make it a pointer for now to update it extrenally 
	//static std::shared_ptr<UniformBuffer> m_CameraMatUBO;
};