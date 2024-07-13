#pragma once
#include "../ObjectBuffer/VertexArray.h"
#include "../ObjectBuffer/VertexBuffer.h"
#include "../ObjectBuffer/IndexBuffer.h"

#include "glm/glm.hpp"


/// <summary>
///TO-DO: Probaly later have something called a shared mesh 
///		   It might be similar to batch rendering i think
///		   so similar mesh wouldnt be duplicated in memory
/// </summary>
class Mesh
{
protected:
	VertexArray VAO;
	VertexBuffer VBO;
	IndexBuffer IBO;

	int m_RefCount = 0;

public:
	Mesh();
	~Mesh();

	virtual void Create();
	void Render();
	void RenderDebugOutLine();

	void Clear();

	void RegisterUse();
	void UnRegisterUse();
	inline int const RefCount() const { return m_RefCount; }
private: 
	void DefaultMesh();

	//friend class SquareMesh;
	//friend class CubeMesh;
	//friend class SphereMesh;
};
