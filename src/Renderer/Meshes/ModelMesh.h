#pragma once

#include "Mesh.h"

class ModelMesh : public Mesh
{
private:
	std::vector<class Texture> m_Textures;
	std::vector<Vertex> m_Vertices;
public:
	ModelMesh() = default;
	virtual void Create() override;

	void Generate(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void NewRender(class Shader& shader);

	std::vector<Vertex> GetVerties() { return m_Vertices; }
	AABB RecalculateAABB();
};