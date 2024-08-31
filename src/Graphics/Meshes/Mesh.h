#pragma once
#include "../ObjectBuffer/VertexArray.h"
#include "../ObjectBuffer/VertexBuffer.h"
#include "../ObjectBuffer/IndexBuffer.h"

#include "glm/glm.hpp"

#include <vector>

struct Vertex
{
	float position[4];
	float colour[4];
	float normals[3];
	float texCoord[2];
};


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
public:
	Mesh();
	Mesh(const VertexArray vao, VertexBuffer vbo);
	~Mesh();

	virtual void Create();
	void Render();
	void RenderArray();
	void RenderDebugOutLine();

	void Clear();

	void RegisterUse();
	void UnRegisterUse();
	inline int const RefCount() const { return m_RefCount; }

	inline std::vector<Vertex>* const Ptr_Vertices() { return &m_Vertices; }
	inline std::vector<Vertex> const GetVertices() { return m_Vertices; }

	inline VertexArray* const GetVAO() { return &VAO; }
	inline IndexBuffer* const GetIBO() { return &IBO; }

	inline unsigned int const GetVerticesCount() const { return m_Vertices.size();}
private: 
	void DefaultMesh();

	//friend class SquareMesh;
	//friend class CubeMesh;
	//friend class SphereMesh;
};
