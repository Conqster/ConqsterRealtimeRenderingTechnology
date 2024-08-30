#include "UniformBuffer.h"

#include "..\RendererErrorAssertion.h"

UniformBuffer::UniformBuffer(signed long long int size)
{
	Generate(size);
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &m_ID);
}

void UniformBuffer::Generate(signed long long int size)
{
	glGenBuffers(1, &m_ID);

	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//TRY TO COMMENT OUT BELOW LATER 
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_ID, 0, size);
}

void UniformBuffer::Bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
}

void UniformBuffer::UnBind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::SetBufferSubData(unsigned offset, signed long long int size, const void* data) const
{
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}
