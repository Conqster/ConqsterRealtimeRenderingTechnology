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
	// need to configure which block to bind to see BindBufferRndIdx
	//GLCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_ID, 0, size));
}

void UniformBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_ID));
}

void UniformBuffer::UnBind() const
{
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::SetBufferSubData(const void* data, unsigned long long int size, unsigned int offset) const
{
	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
}

void UniformBuffer::BindBufferRndIdx(const unsigned int block_idx, unsigned long long int size, unsigned int offset)
{
	m_BlockIdx = block_idx;
	GLCall(glBindBufferRange(GL_UNIFORM_BUFFER, block_idx, m_ID, offset, size));
}

void UniformBuffer::SetSubDataByID(const void* data, signed long long int size, unsigned int offset) const
{
	GLCall(glNamedBufferSubData(m_ID, offset, size, data));
}

void UniformBuffer::Delete()
{
	GLCall(glDeleteBuffers(1, &m_ID));
}
