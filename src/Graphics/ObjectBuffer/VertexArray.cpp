#pragma once
#include "Graphics/RendererErrorAssertion.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"


void VertexArray::Generate()
{
	GLCall(glGenVertexArrays(1, &m_Id));
}

void VertexArray::AddBufferLayout(const VertexBuffer& vBuffer, const VertexBufferLayout& layout)
{
	Bind();
	vBuffer.Bind();

	size_t count = 0;
	const auto& elements = layout.GetElements();
	GLsizei mStride = layout.GetStride();
	unsigned int offset = 0;

	for (size_t i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalised, mStride, (const void*)offset));

		offset += element.count * VertexBufferElement::GetTypeSize(element.type);
		//offset += element.count * element.GetSize();
		m_ArrayCount++;
	}



	//GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0));
	//GLCall(glEnableVertexAttribArray(0));
	//GLCall(glEnableVertexAttribArray(1));
	//GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 4)));


}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_Id));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}

VertexArray::~VertexArray()
{
	//Clean();
}

void VertexArray::Clean()
{
	//NEED TO UNBIND CURRENT BINDED BUFFER AS TO NOT DISABLE AND DELETE WRONG BUFFER 
	if (m_ArrayCount && m_Id)
	{
		//Unbind();
		//Bind();
		// I CAN glDisableVertexAttribArray(i) ON LAST GAMEOBJECT
		//for (unsigned int i = 0; i < m_ArrayCount; i++)
		//{
		//	GLCall(glDisableVertexAttribArray(i));
		//}
	GLCall(glDeleteVertexArrays(1, &m_Id));
	}
}

