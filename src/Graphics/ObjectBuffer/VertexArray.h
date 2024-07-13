#pragma once
//#include "VertexBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_Id = 0;
	unsigned int m_ArrayCount = 0;
public:
	VertexArray() = default;

	void Generate();
	void AddBufferLayout(const class VertexBuffer& vBuffer, const class VertexBufferLayout& layout);
	void Bind() const;
	void Unbind() const;

	~VertexArray();
	void Clean();

};
