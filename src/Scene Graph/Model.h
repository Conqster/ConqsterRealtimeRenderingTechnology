#pragma once
#include <vector>

#include "Renderer/Meshes/ModelMesh.h"
#include "Renderer/Texture.h"

#include "Geometry/AABB.h"

class Model
{
public:
	Model() = default;
	Model(std::vector<ModelMesh> _meshes);

	void Draw();
	void Draw(class Shader& shader);
	void DebugWireDraw();

	void Destroy();
	inline AABB GetAABB() { return aabb; }
	void RecalculateAABBFromMesh();
	 std::vector<ModelMesh> GetMeshes() { return meshes; }
	//~Model() = default;
private: 
	std::vector<ModelMesh> meshes;
	std::vector<Texture> textures;

	std::string dir = "";
	AABB aabb;
	void UpdateAABB(const AABB& inRHS);
};