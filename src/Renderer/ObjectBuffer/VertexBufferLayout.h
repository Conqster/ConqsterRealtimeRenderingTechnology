#pragma once

#include <vector>
#include "../RendererErrorAssertion.h"

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned int normalised;

	inline unsigned int GetSize() const { return this->GetTypeSize(type); }

	static unsigned int GetTypeSize(unsigned int type)
	{
		switch (type)
		{
			case GL_FLOAT:			return 4;
			case GL_UNSIGNED_INT:	return 4;
			case GL_UNSIGNED_BYTE:	return 1;
		}

		ASSERT(false);
		return 0;
	}
};


class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;

public: 		
	VertexBufferLayout()
		:m_Stride(0){}


	template<typename T>
	void Push(unsigned int count)
	{
		//static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count)
	{
		//VertexBufferElement newElement{ GL_FLOAT, count, GL_FALSE };
		//m_Elements.push_back(newElement);
		//m_Stride += VertexBufferElement::GetTypeSize(GL_FLOAT);
		m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
		m_Stride += count * VertexBufferElement::GetTypeSize(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		//VertexBufferElement newElement{ GL_UNSIGNED_INT, count, GL_FALSE };
		//m_Elements.push_back(newElement);
		m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE });
		//m_Stride += VertexBufferElement::GetTypeSize(GL_UNSIGNED_INT);
		m_Stride += count * VertexBufferElement::GetTypeSize(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		//VertexBufferElement newElement{ GL_UNSIGNED_BYTE, count, GL_TRUE };
		//m_Elements.push_back(newElement);
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		//m_Stride += VertexBufferElement::GetTypeSize(GL_UNSIGNED_BYTE);
		m_Stride += count * VertexBufferElement::GetTypeSize(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }

};
