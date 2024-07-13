#include "Mesh.h"
#include "GL/glew.h"
#include "../ObjectBuffer/VertexBufferLayout.h"

//TO-DO: might change/move GLCall helper
//       to a helper class from Renderer 
//		 avoid circle include
#include "Graphics/Renderer.h"
#include <iostream>

#include "Util/MathsHelpers.h"

Mesh::Mesh()
{
	//Create();
	m_RefCount = 0;
}

Mesh::~Mesh()
{
	//Clear();
}

void Mesh::Create()
{
	DefaultMesh();
}

void Mesh::Render()
{
	VAO.Bind();
	IBO.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, IBO.GetCount(), GL_UNSIGNED_INT, (void*)0));
}

void Mesh::RenderDebugOutLine()
{
	VAO.Bind();
	IBO.Bind();
	glLineWidth(2.0f);
	GLCall(glDrawElements(GL_LINE_LOOP, IBO.GetCount(), GL_UNSIGNED_INT, (void*)0));
	glLineWidth(1.0f);
}


void Mesh::Clear()
{
	VBO.DeleteBuffer();
	IBO.DeleteBuffer();

	VAO.Clean();
	std::cout << "Cleared mesh\n";
}

void Mesh::RegisterUse()
{
	m_RefCount++;
}

void Mesh::UnRegisterUse()
{
	m_RefCount--;
	if(m_RefCount <= 0)
		Clear();
}



/// <summary>
/// Use to default mesh to triangle
/// </summary>
void Mesh::DefaultMesh()
{
	//triangle
	std::cout << "Default mesh called!!!!!!!" << std::endl;
	//50.0f >> 1.0f
	float vertices[] =
	{//			  x		 y	   z	 w      	 r      g    b     a		u     v
		/*0*/	-1.0f, -1.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
		/*1*/	0.0f, 1.0f, 0.0f, 1.0f,			0.0f, 1.0f, 0.0f, 1.0f,    0.5f, 0.0f,
		/*2*/	1.0f, -1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f,

		//EXTRA POS
		/*3*/	1.0f, 1.0f, 0.0f, 1.0f,			0.2f, 0.8f, 0.0f, 1.0f,	   1.0f, 0.0f, //top-right
		/*4*/	-1.0f, 1.0f, 0.0f, 1.0f,		0.0f, 0.2f, 0.8f, 1.0f,    0.0f, 0.0f,  //top-left

	};


	 unsigned int indices[] =
	{

		0, 3, 2
		//0, 3, 4,
		//0, 1, 2,
	};


	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	VAO.Generate();

	VBO = VertexBuffer(vertices, sizeof(vertices));
	IBO = IndexBuffer(indices, /*6*/3);
	VertexBufferLayout vbLayout;
	//layout 0 position 4 floats x, y, z, w
	vbLayout.Push<float>(4);
	//layout 1 colour 4 floats r, g, b, a
	vbLayout.Push<float>(4);
	//texture coord floats u v
	vbLayout.Push<float>(2);
	VAO.AddBufferLayout(VBO, vbLayout);
}

