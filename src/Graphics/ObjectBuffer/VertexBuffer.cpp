#pragma once
#include "VertexBuffer.h"
#include "Graphics/RendererErrorAssertion.h"

#include <iostream>

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	GLCall(glGenBuffers(1, &m_Id));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_Id));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	//GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	//GLCall(glDeleteBuffers(1, &m_Id));
	//std::cout << "Vertex buffer destroyesd\n";
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_Id));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::DeleteBuffer() const
{
	Unbind();
	GLCall(glDeleteBuffers(1, &m_Id));
}
