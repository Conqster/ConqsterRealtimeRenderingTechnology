#pragma once
#include <memory>



class Mesh;
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