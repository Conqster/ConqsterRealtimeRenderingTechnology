#pragma once

class UniformBuffer
{
	unsigned int m_ID = 0;

public:
	UniformBuffer() = default;
	UniformBuffer(signed long long int size);
	~UniformBuffer();

	void Generate(signed long long int size);
	void Bind() const;
	void UnBind() const;
	void SetBufferSubData(unsigned offset, signed long long int size, const void* data) const;

};