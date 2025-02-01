#pragma once

#include "glm/glm.hpp"
#include "Shader.h"
#include "ObjectBuffer/UniformBuffer.h"
#include "Meshes/Mesh.h"

#include "SceneRenderer.h"

#include <memory>




class DebugGizmos
{
public:
	DebugGizmos() = default;
	~DebugGizmos();

	static void Startup();
	
	//Primary
	static void DrawTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec4 colour = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), bool cull_face = true);
	static void DrawTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec4 col0 = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec4 col1 = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec4 col2 = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),bool cull_face = true);
	static void DrawWireTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawLine(glm::vec3 v0, glm::vec3 v1, glm::vec3 colour = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawLine(glm::vec3 v0, glm::vec3 v1, glm::vec4 col0 = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec4 col1 = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), float thickness = 1.0f);
	
	//Custom
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
	static void DrawPlane(const class Plane& f, glm::vec2 size = glm::vec2(10.0f, 10.0f), glm::vec3 col = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawWirePlane(const Plane& f, glm::vec2 size = glm::vec2(10.0f, 10.0f), glm::vec3 col = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void DrawFrustum(const class Frustum& f, glm::vec3 col = glm::vec3(1.0f, 0.0f, 1.0f), float thickness = 1.0f);
	static void Cleanup();

	static void DrawAllBatches();
private:
	static bool active;
	static bool use;
	static Mesh sphere;


	static Shader m_DebugMeshShader;
	static Shader m_BatchLineShader;
	static Shader m_InstanceShader;

	static CRRT::SceneRenderer m_Renderer;
	static void Generate();

#pragma region Helper strutures

	struct Line
	{
		float start[3]; float startColour[4];  // <--- vertex0
		float end[3]; float endColour[4];	   // <--- vertex1
	};
	struct Triangle
	{
		float v0[3]; float v0Colour[4];			// <--- vertex0
		float v1[3]; float v1Colour[4];			// <--- vertex1
		float v2[3]; float v2Colour[4];			// <--- vertex2
	};

	struct LineSegmentVertex
	{
		unsigned int VAO;
		unsigned int VBO;

		unsigned int curr_vertex_count = 1000;
	};
	/* VertexData (GPU info)
	* vao --> vertex attrib
	* ibo --> static draw idx buffer
	* vbo --> static draw vertex buffer
	* 
	* indices_count  --> cache draw idx count
	* vbo2 --> dyanamic draw instance vertex buffer
	* curr_inst_count --> cache instance data count 
						  for GPU buffer allocation.
	*/
	struct VertexObjData
	{
		unsigned int vao;
		unsigned int vbo;
		unsigned int ibo;

		//... extra
		unsigned int indices_count = 0;
		unsigned int vbo2;
		//utility count could be used for num of vertex or 
		//num of instances 
		unsigned int curr_inst_count = 500;
	};
	struct VertexInstanceData
	{
		glm::vec4 colour;
		glm::mat4 transform;
	};

#pragma endregion

	//Vertex helpers
	static LineSegmentVertex m_LineSegVertex;
	static VertexObjData m_SphereVertexData;


	//later cache in array, without destruction
	static std::vector<Line> m_BatchLines;
	static std::vector<Triangle> m_BatchTriangles;
	static std::vector<Triangle> m_BatchNoCullFaceTriangles;
	static std::vector<VertexInstanceData> m_SolidSphereInstance;
	static std::vector<VertexInstanceData> m_WireSphereInstance;


	
	static void BatchLineSegment(const Line& line);
	static void BatchTriangle(const Triangle& tri, bool cull_face = true);
	static void DrawInstances();
	static void CacheWireSphereInstance(const VertexInstanceData& trans);
	static void CacheSolidSphereInstance(const VertexInstanceData& trans);
};