#pragma once
#include "../ObjectBuffer/VertexArray.h"
#include "../ObjectBuffer/VertexBuffer.h"
#include "../ObjectBuffer/IndexBuffer.h"

#include "glm/glm.hpp"

#include <vector>

#include "Geometry/AABB.h"

struct Vertex
{
	float position[4];
	float colour[4];
	float texCoord[2];
	float normals[3];

	//normal/bump mapping
	//maybe have a flag so it does not have tangent 
	//on create in gpu dont send full vertex size anf tan& bitan
	float tangent[3];
	float bitangent[3];
};

///A Mesh does not know about materials
/// <summary>
///TO-DO: Probaly later have something called a shared mesh 
///		   It might be similar to batch rendering i think
///		   so similar mesh wouldnt be duplicated in memory
/// </summary>
class Mesh
{
protected:
	std::vector<Vertex> m_Vertices;

	VertexArray VAO;
	VertexBuffer VBO;
	IndexBuffer IBO;

	int m_RefCount = 0;
	void CacheVertices(const float vertices[], size_t size);
	void ReCalcNormalsWcIndices(float* vertices, unsigned int* indices, unsigned int vertices_count,unsigned int indices_count, 
								unsigned int vertex_stride, unsigned int normal_offset, bool reset_normals = true);

	AABB aabb;
	void UpdateAABB(glm::vec3 v);
public:
	Mesh();
	Mesh(const VertexArray vao, VertexBuffer vbo);
	void Generate(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	~Mesh();

	virtual void Create();
	void Render();
	void RenderInstances(int count);
	void RenderDebugOutLine(float thickness = 1.5f);

	void Clear();

	void RegisterUse();
	void UnRegisterUse();
	inline int const RefCount() const { return m_RefCount; }

	inline std::vector<Vertex>* const Ptr_Vertices() { return &m_Vertices; }
	inline std::vector<Vertex> const GetVertices() { return m_Vertices; }

	inline VertexArray* const GetVAO() { return &VAO; }
	inline IndexBuffer* const GetIBO() { return &IBO; }

	inline unsigned int const GetVerticesCount() const { return m_Vertices.size();}

	inline AABB GetAABB() { return aabb; }
private: 
	void DefaultMesh();
};
