#pragma once

#include "Mesh.h"
#include <memory>

struct Material;
class ModelMesh : public Mesh
{
private:
	std::vector<class Texture> m_Textures;
	std::vector<Vertex> m_Vertices;

	std::shared_ptr<Material> m_Material = nullptr;
public:
	ModelMesh() = default;
	virtual void Create() override;

	void Generate(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void NewRender(class Shader& shader);

	std::vector<Vertex> GetVerties() { return m_Vertices; }
	AABB RecalculateAABB();

	inline std::shared_ptr<Material> const GetMaterial() const { return m_Material; }


	//New New
	void Generate(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Material> mat);
};



