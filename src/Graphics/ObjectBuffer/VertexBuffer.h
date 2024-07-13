#pragma once

class VertexBuffer
{
private:
	unsigned int m_Id = 0;

public:
	VertexBuffer() = default;
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
	void DeleteBuffer() const;
};