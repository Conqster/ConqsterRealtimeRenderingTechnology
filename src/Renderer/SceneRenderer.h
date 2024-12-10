#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

class Mesh;
class Material;
struct RenderableMesh
{
	const std::weak_ptr<Mesh> mesh;
	const std::weak_ptr<Material> material;
	glm::mat4* transform = nullptr;
	unsigned int objectID = 0;
	float m_SqrViewDist = 0.0f;

	void UpdateViewSqrDist(const glm::vec3& view_pos)
	{
		m_SqrViewDist = glm::length2(view_pos - glm::vec3((*transform)[3]));
	}

	static inline bool CompareDistanceToView(const std::weak_ptr<RenderableMesh>& a, const std::weak_ptr<RenderableMesh>& b)
	{
		auto ptr_a = a.lock();
		auto ptr_b = b.lock();
		if (!ptr_a || !ptr_b)
			return false;

		return (ptr_a->m_SqrViewDist > ptr_b->m_SqrViewDist);
	}
};

class VertexArray;
class IndexBuffer;
namespace CRRT
{
	class SceneRenderer
	{
	public:
		//void Init();
		void Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		//---------------------Draw---------------------------------/
		void DrawMesh(const std::shared_ptr<Mesh>& mesh);
		void DrawMesh(Mesh& mesh);
		void DrawVertexElements(const VertexArray vao, const IndexBuffer ibo);
		void DrawVertexElementsOutline(const VertexArray vao, const IndexBuffer ibo, float thinkness = 1.0f);
		void DrawMeshOutline(Mesh& mesh, float thinkness = 1.0f);
		void DrawMeshInstance(const std::shared_ptr<Mesh>& mesh, int count);
		void DrawMeshInstance(Mesh& mesh, int count);
	};
}