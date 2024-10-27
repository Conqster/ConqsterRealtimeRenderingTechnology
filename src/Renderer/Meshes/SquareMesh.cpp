#include "SquareMesh.h"
#include "GL/glew.h"
#include "../ObjectBuffer/VertexBufferLayout.h"

void SquareMesh::Create()
{
	float vertices[] =
	{//			  x		 y	   z	 w      	 r      g    b     a		u     v      nx     ny      nz

		/*0*/	-1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		/*1*/	-1.0f, -1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,    0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		/*2*/	1.0f, -1.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f, 1.0f,    1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		/*3*/	1.0f, 1.0f, 0.0f, 1.0f,	 		0.0f, 0.2f, 0.8f, 1.0f,    1.0f, 1.0f,	0.0f,  0.0f,  1.0f,
	};

	unsigned int indices[] =
	{
		0, 1, 2, 
		0, 2, 3
	};



	////For cal Tangent & Bitagent
	glm::vec3 p0(-1.0f, 1.0f, 0.0f),
			  p1(-1.0f, -1.0f, 0.0f),
			  p2(1.0f, -1.0f, 0.0f),
			  p3(1.0f, 1.0f, 0.0f);
	
	glm::vec2 uv0(0.0f, 1.0f),
			  uv1(0.0f, 0.0f),
			  uv2(1.0f, 0.0f),
			  uv3(1.0f, 1.0f);

	glm::vec3 nor(0.0f, 0.0f, 1.0f);

	glm::vec3 t0, t1, 
			  bt0, bt1;

	//Tri 1
	glm::vec3 edge1 = p1 - p0,	
			  edge2 = p2 - p0;
	glm::vec2 deltaUV0 = uv1 - uv0,
			  deltaUV1 = uv2 - uv0;

	float f = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

	t0.x = f * (deltaUV1.y * edge1.x - deltaUV0.y * edge2.x);
	t0.y = f * (deltaUV1.y * edge1.y - deltaUV0.y * edge2.y);
	t0.z = f * (deltaUV1.y * edge1.z - deltaUV0.y * edge2.z);

	bt0.x = f * (-deltaUV1.x * edge1.x + deltaUV0.x * edge2.x);
	bt0.y = f * (-deltaUV1.x * edge1.y + deltaUV0.x * edge2.y);
	bt0.z = f * (-deltaUV1.x * edge1.z + deltaUV0.x * edge2.z);


	//Tri 2
	edge1 = p2 - p0;
	edge2 = p3 - p0;
	deltaUV0 = uv2 - uv0;
	deltaUV1 = uv3 - uv0;

	f = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

	t1.x = f * (deltaUV1.y * edge1.x - deltaUV0.y * edge2.x);
	t1.y = f * (deltaUV1.y * edge1.y - deltaUV0.y * edge2.y);
	t1.z = f * (deltaUV1.y * edge1.z - deltaUV0.y * edge2.z);

	bt1.x = f * (-deltaUV1.x * edge1.x + deltaUV0.x * edge2.x);
	bt1.y = f * (-deltaUV1.x * edge1.y + deltaUV0.x * edge2.y);
	bt1.z = f * (-deltaUV1.x * edge1.z + deltaUV0.x * edge2.z);


	//Reupdate Vertices
	float newVertices[] =
	{
				//pos						//col					 //uv			//nor				  //tan               //bitan
		/*0*/	p0.x, p0.y, p0.z, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,  uv0.x, uv0.y,   nor.x, nor.y, nor.z,  t0.x, t0.y, t0.z,  bt0.x, bt0.y, bt0.z,
		/*1*/	p1.x, p1.y, p1.z, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,  uv1.x, uv1.y,   nor.x, nor.y, nor.z,  t0.x, t0.y, t0.z,  bt0.x, bt0.y, bt0.z,
		/*2*/	p2.x, p2.y, p2.z, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,  uv2.x, uv2.y,   nor.x, nor.y, nor.z,  t0.x, t0.y, t0.z,  bt0.x, bt0.y, bt0.z,


		/*3*/	p0.x, p0.y, p0.z, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,  uv0.x, uv0.y,   nor.x, nor.y, nor.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
		/*4*/	p2.x, p2.y, p2.z, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,  uv2.x, uv2.y,   nor.x, nor.y, nor.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
		/*5*/	p3.x, p3.y, p3.z, 1.0f,		1.0f, 0.0f, 0.0f, 1.0f,  uv3.x, uv3.y,   nor.x, nor.y, nor.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
	};

	unsigned int newIndices[] =
	{
		0, 1, 2, 
		3, 4, 5
	};


	int vertices_count = sizeof(vertices) / sizeof(vertices[0]);
	int indice_count = sizeof(indices) / sizeof(indices[0]);
	ReCalcNormalsWcIndices(vertices, indices, vertices_count, indice_count, 13, 10);
	CacheVertices(vertices, sizeof(vertices));

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	VAO.Generate();

	//VBO = VertexBuffer(vertices, sizeof(vertices));
	VBO = VertexBuffer(newVertices, sizeof(newVertices));
	//IBO = IndexBuffer(indices, 6);
	IBO = IndexBuffer(newIndices, 6);
	VertexBufferLayout vbLayout;
	//layout 0 position 4 floats x, y, z, w
	vbLayout.Push<float>(4);
	//layout 1 colour 4 floats r, g, b, a
	vbLayout.Push<float>(4);
	//texture coord floats u v
	vbLayout.Push<float>(2);
	//normals nx, ny, nz
	vbLayout.Push<float>(3);
	//tangent tx, ty, tz
	vbLayout.Push<float>(3);
	//Bitangent btx, bty, btz
	vbLayout.Push<float>(3);
	VAO.AddBufferLayout(VBO, vbLayout);
}
