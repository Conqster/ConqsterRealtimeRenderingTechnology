#include "UniformBuffer.h"

#include "..\RendererErrorAssertion.h"

UniformBuffer::UniformBuffer(signed long long int size)
{
	Generate(size);
}

UniformBuffer::~UniformBuffer()
{
	Delete();
}

void UniformBuffer::Generate(signed long long int size)
{
	GLCall(glGenBuffers(1, &m_ID));

	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_ID));
	GLCall(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

	//TRY TO COMMENT OUT BELOW LATER 
	GLCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_ID, 0, size));
}

void UniformBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_ID));
}

void UniformBuffer::UnBind() const
{
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::SetBufferSubData(unsigned offset, signed long long int size, const void* data) const
{
	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
}

void UniformBuffer::Delete()
{
	GLCall(glDeleteBuffers(1, &m_ID));
}
