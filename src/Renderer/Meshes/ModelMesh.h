#pragma once

#include "Mesh.h"
#include <memory>

struct Material;
/// <summary>
/// Deprecated Class, Entity is Object class now
/// </summary>
class ModelMesh
{
private:
	std::vector<Vertex> m_Vertices;

	VertexArray VAO;
	VertexBuffer VBO;
	IndexBuffer IBO;

	std::vector<class Texture> m_Textures;
	std::shared_ptr<Material> m_Material = nullptr;

	AABB aabb;
public:
	ModelMesh() = default;
	void Generate(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	//New New
	void Generate(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Material> mat);

	void Create();
	AABB RecalculateAABB();
	void Clear();

	void NewRender(class Shader& shader);

	std::vector<Vertex> GetVerties() { return m_Vertices; }
	inline VertexArray const GetVAO() { return VAO; }
	inline VertexBuffer const GetVBO() { return VBO; }
	inline IndexBuffer const GetIBO() { return IBO; }
	inline AABB GetAABB() { return aabb; }
	inline std::shared_ptr<Material> const GetMaterial() const { return m_Material; }
private:
	void UpdateAABB(glm::vec3 v);
};



