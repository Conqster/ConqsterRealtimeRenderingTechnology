#include "SquareMesh.h"
#include "GL/glew.h"
#include "../ObjectBuffer/VertexBufferLayout.h"

void SquareMesh::Create()
{

	//m_Vertices.push_back({ -0.5f, -0.5f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f,  0.0f,  0.0f,  1.0f, });

	//50.0f >> 1.0f
	float vertices[] =
	{//			  x		 y	   z	 w      	 r      g    b     a		u     v      nx     ny      nz
		/*0*/	-0.5f, -0.5f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f,  0.0f,  0.0f,  1.0f, 
		/*1*/	0.0f, 0.5f, 0.0f, 1.0f,			0.0f, 1.0f, 0.0f, 1.0f,    0.5f, 0.0f,  0.0f,  0.0f,  1.0f,
		/*2*/	0.5f, -0.5f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 1.0f,  0.0f,  0.0f,  1.0f,

		//EXTRA POS
		/*3*/	0.5f, 0.5f, 0.0f, 1.0f,			0.2f, 0.8f, 0.0f, 1.0f,	   1.0f, 0.0f,	0.0f,  0.0f,  1.0f,	//top-right
		/*4*/	-0.5f, 0.5f, 0.0f, 1.0f,	 	0.0f, 0.2f, 0.8f, 1.0f,    0.0f, 0.0f,	0.0f,  0.0f,  1.0f,		//top-left

	};

	CacheVertices(vertices, sizeof(vertices));


	//m_Vertices[0].position[0] = 0.0f;

	unsigned int indices[] =
	{

		//0, 1, 3,
		//3, 0, 4,
		//0, 2, 3,
		4, 0, 3, 
		0, 2, 3,
		//3, 0, 4,
	};


	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	VAO.Generate();

	VBO = VertexBuffer(vertices, sizeof(vertices));
	IBO = IndexBuffer(indices, 6);
	VertexBufferLayout vbLayout;
	//layout 0 position 4 floats x, y, z, w
	vbLayout.Push<float>(4);
	//layout 1 colour 4 floats r, g, b, a
	vbLayout.Push<float>(4);
	//texture coord floats u v
	vbLayout.Push<float>(2);
	//normals nx, ny, nz
	vbLayout.Push<float>(3);
	VAO.AddBufferLayout(VBO, vbLayout);
}
