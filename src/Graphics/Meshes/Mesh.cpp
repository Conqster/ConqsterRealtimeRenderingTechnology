#include "Mesh.h"
#include "GL/glew.h"
#include "../ObjectBuffer/VertexBufferLayout.h"

//TO-DO: might change/move GLCall helper
//       to a helper class from Renderer 
//		 avoid circle include
#include "Graphics/RendererErrorAssertion.h"
#include <iostream>

#include "Util/MathsHelpers.h"

#include "../Texture.h"
#include "../Shader.h"

void Mesh::CacheVertices(const float vertices[], size_t size)
{
	//unsigned int size = sizeof(vertices);
	unsigned int item_size = sizeof(vertices[0]);
	unsigned int count = size / item_size;
	unsigned int num_of_vertices = count / 13;
	auto& v = vertices;

	for (size_t i = 0; i < num_of_vertices; i++)
	{
		m_Vertices.push_back(
			{ v[0 + (i * 13)], v[1 + (i * 13)], v[2 + (i * 13)], v[3 + (i * 13)], //pos
			v[4 + (i * 13)], v[5 + (i * 13)], v[6 + (i * 13)], v[7 + (i * 13)],   //col
			v[10 + (i * 13)], v[11 + (i * 13)], v[12 + (i * 13)],                   // now nor swap uv
			v[8 + (i * 13)], v[9 + (i * 13)], });								  // now uv swap nor
	}
}

Mesh::Mesh()
{
	//Create();
	m_RefCount = 0;
}

Mesh::Mesh(const VertexArray vao, VertexBuffer vbo)
{
	VAO = vao;
	VBO = vbo;
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
	VAO.Unbind();
	IBO.Unbind();
}

void Mesh::RenderInstances(int count)
{
	VAO.Bind();
	IBO.Bind();
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, IBO.GetCount(), GL_UNSIGNED_INT, (void*)0, count));
}



void Mesh::RenderDebugOutLine()
{
	VAO.Bind();
	IBO.Bind();
	GLCall(glLineWidth(2.0f));
	GLCall(glDrawElements(GL_LINE_LOOP, IBO.GetCount(), GL_UNSIGNED_INT, (void*)0));
	GLCall(glLineWidth(1.0f));
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

	CacheVertices(vertices, sizeof(vertices));

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

