#pragma once
#include "IndexBuffer.h"
#include "Graphics/RendererErrorAssertion.h"
#include <iostream>

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	:m_Count(count)
{

	GLCall(glGenBuffers(1, &m_Id));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int/*GLuint*//*data[0]*/) * count, data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	//GLCall(glDeleteBuffers(1, &m_Id));
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id));
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::DeleteBuffer() const
{
	Unbind();
	GLCall(glDeleteBuffers(1, &m_Id));
}
