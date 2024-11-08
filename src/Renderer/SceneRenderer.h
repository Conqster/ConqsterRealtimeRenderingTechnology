#pragma once
#include <memory>



class Mesh;
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
		void DrawMeshOutline(Mesh& mesh, float thinkness = 1.0f);
		void DrawMeshInstance(const std::shared_ptr<Mesh>& mesh, int count);
		void DrawMeshInstance(Mesh& mesh, int count);
	};
}