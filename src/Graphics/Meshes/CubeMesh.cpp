#include "CubeMesh.h"
#include "GL/glew.h"
#include "../ObjectBuffer/VertexBufferLayout.h"

void CubeMesh::Create()
{


	float old_vertices[] =
	{
		//		  x	      y       z     w       r		g	  b    a   u    v		     nx	   ny     nz		  nx	ny     nz
		/*0*/	-0.5f, -0.5f, 0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 1.0f,  		1.0f, -1.0f, 1.0f,
		/*1*/	0.5f, -0.5f, 0.5f, 1.0f,	1.0f, 1.0f, 0.0f, 1.0f,	  1.0f, 1.0f,		1.0f, -1.0f, 1.0f,
		/*2*/	0.5f, 0.5f, 0.5f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,	  1.0f, 0.0f,		1.0f, 1.0f, 1.0f,
		/*3*/	-0.5f, 0.5f, 0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 0.0f,		1.0f, 1.0f, 1.0f,


		/*4*/	-0.5f, -0.5f, -0.5f, 1.0f,	1.0f, 1.0f, 1.0f, 1.0f,	  0.0f, 0.0f,		-1.0f, -1.0f, -1.0f,
		/*5*/	0.5f, 0.5f, -0.5f, 1.0f,	1.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 1.0f,		 1.0f,  1.0f, -1.0f,
		/*6*/	0.5f, -0.5f, -0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 0.0f,		 1.0f, -1.0f, -1.0f,
		/*7*/	-0.5f, 0.5f, -0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,		-1.0f,  1.0f, -1.0f,



		/*0a or 8*/		-0.5f, -0.5f, 0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 1.0f,  		-1.0f, -1.0f, 1.0f,
		/*1a or 9*/		0.5f, -0.5f, 0.5f, 1.0f,	1.0f, 1.0f, 0.0f, 1.0f,	  1.0f, 1.0f,		 1.0f, -1.0f, 1.0f,
		/*2a or 10*/	0.5f, 0.5f, 0.5f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,	  1.0f, 0.0f,		 1.0f,  1.0f, 1.0f,
		/*3a or 11*/	-0.5f, 0.5f, 0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 0.0f,		-1.0f,  1.0f, 1.0f,


		/*4a or 12*/	-0.5f, -0.5f, -0.5f, 1.0f,	1.0f, 1.0f, 1.0f, 1.0f,	  0.0f, 0.0f,		-1.0f, -1.0f, -1.0f,
		/*5a or 13*/	0.5f, 0.5f, -0.5f, 1.0f,	1.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 1.0f,		 1.0f,  1.0f, -1.0f,
		/*6a or 14*/	0.5f, -0.5f, -0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 0.0f,		 1.0f, -1.0f, -1.0f,
		/*7a or 15*/	-0.5f, 0.5f, -0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,		-1.0f,  1.0f, -1.0f,


		/*0b or 16*/	-0.5f, -0.5f, 0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,   1.0f, 1.0f,  		-1.0f, -1.0f, 1.0f,
		/*1b or 17*/	0.5f, -0.5f, 0.5f, 1.0f,	1.0f, 1.0f, 0.0f, 1.0f,	  0.0f, 1.0f,		 1.0f, -1.0f, 1.0f,
		/*2b or 18*/	0.5f, 0.5f, 0.5f, 1.0f,		0.0f, 1.0f, 0.0f, 1.0f,	  0.0f, 0.0f,		 1.0f,  1.0f, 1.0f,
		/*3b or 19*/	-0.5f, 0.5f, 0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  1.0f, 0.0f,		-1.0f,  1.0f, 1.0f,


		/*4b or 20*/	-0.5f, -0.5f, -0.5f, 1.0f,	1.0f, 1.0f, 1.0f, 1.0f,	  0.0f, 1.0f,		-1.0f, -1.0f, -1.0f,
		/*5b or 21*/	0.5f, 0.5f, -0.5f, 1.0f,	1.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 0.0f,		 1.0f,  1.0f, -1.0f,
		/*6b or 22*/	0.5f, -0.5f, -0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 1.0f,		 1.0f, -1.0f, -1.0f,
		/*7b or 23*/	-0.5f, 0.5f, -0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 0.0f,		-1.0f,  1.0f, -1.0f,
	};


	float vertices[] =
	{
		//				 x	    y       z     w       r		g	  b    a		u    v		     nx	   ny     nz	
		// Front face (+z)
		/*0*/			-0.5f, -0.5f, 0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f,  		0.0f, 0.0f,  1.0f,	
		/*1*/			 0.5f, -0.5f, 0.5f, 1.0f,	1.0f, 1.0f, 0.0f, 1.0f,	  1.0f, 0.0f,		0.0f, 0.0f,  1.0f,	
		/*2*/			 0.5f,  0.5f, 0.5f, 1.0f,	0.0f, 1.0f, 0.0f, 1.0f,	  1.0f, 1.0f,		0.0f, 0.0f,  1.0f, 	
		/*3*/			-0.5f,  0.5f, 0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,		0.0f, 0.0f,  1.0f,	
																													
		// Back face (-z)
		/*4*/			-0.5f, -0.5f, -0.5f, 1.0f,	1.0f, 1.0f, 1.0f, 1.0f,	  0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		
		/*5*/			 0.5f, -0.5f, -0.5f, 1.0f,	1.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		
		/*6*/			 0.5f,  0.5f, -0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 1.0f,		0.0f, 0.0f, -1.0f,		
		/*7*/			-0.5f,  0.5f, -0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,		0.0f, 0.0f, -1.0f, 		


		// Right face (+x)
		/*0a or 8*/		0.5f, -0.5f, -0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f,  		1.0f, 0.0f, 0.0f,		
		/*1a or 9*/		0.5f,  0.5f, -0.5f, 1.0f,	1.0f, 1.0f, 0.0f, 1.0f,	  1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		
		/*2a or 10*/	0.5f,  0.5f,  0.5f, 1.0f,	0.0f, 1.0f, 0.0f, 1.0f,	  1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		
		/*3a or 11*/	0.5f, -0.5f,  0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,		1.0f, 0.0f, 0.0f,		

		//Left face  (-x)
		/*4a or 12*/	-0.5f, -0.5f, -0.5f, 1.0f,	1.0f, 1.0f, 1.0f, 1.0f,	  0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,    
		/*5a or 13*/	-0.5f,  0.5f, -0.5f, 1.0f,	1.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,    
		/*6a or 14*/	-0.5f,  0.5f,  0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,    
		/*7a or 15*/	-0.5f, -0.5f,  0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,		-1.0f, 0.0f, 0.0f,    

		//Top face (+y)
		/*0b or 16*/	-0.5f, 0.5f, -0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f,  		 0.0f, 1.0f, 0.0f,   
		/*1b or 17*/	 0.5f, 0.5f, -0.5f, 1.0f,	1.0f, 1.0f, 0.0f, 1.0f,	  1.0f, 0.0f,		 0.0f, 1.0f, 0.0f,   
		/*2b or 18*/	 0.5f, 0.5f,  0.5f, 1.0f,	0.0f, 1.0f, 0.0f, 1.0f,	  1.0f, 1.0f,		 0.0f, 1.0f, 0.0f,   
		/*3b or 19*/	-0.5f, 0.5f,  0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,		 0.0f, 1.0f, 0.0f,   

		//Bottom face (-y)
		/*4b or 20*/	-0.5f, -0.5f, -0.5f, 1.0f,	1.0f, 1.0f, 1.0f, 1.0f,	  0.0f, 0.0f,		 0.0f, -1.0f, 0.0f,    
		/*5b or 21*/	 0.5f, -0.5f, -0.5f, 1.0f,	1.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 0.0f,		 0.0f, -1.0f, 0.0f,    
		/*6b or 22*/	 0.5f, -0.5f,  0.5f, 1.0f,	0.0f, 0.0f, 1.0f, 1.0f,	  1.0f, 1.0f,		 0.0f, -1.0f, 0.0f,    
		/*7b or 23*/	-0.5f, -0.5f,  0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 1.0f,	  0.0f, 1.0f,		 0.0f, -1.0f, 0.0f,    
	};


	unsigned int indices[] =
	{
		//Front face
		0, 1, 2,	0, 2, 3, 
		//Back face
		4, 6, 5,	4, 7, 6,
		//Right face
		8, 9, 10,	8, 10, 11, 
		//Left face
		12, 14, 13,	12, 15, 14,
		//Top face
		16, 18, 17,	16, 19, 18,
		//Bottom face
		20, 21, 22,	20, 22, 23
	};

	int vertices_count = sizeof(vertices) / sizeof(vertices[0]);
	int indice_count = sizeof(indices) / sizeof(indices[0]);
	ReCalcNormalsWcIndices(vertices, indices, vertices_count, indice_count, 13, 10);
	CacheVertices(vertices, sizeof(vertices));


	//Uodate AABB 
	unsigned int pos_offset = 0;
	unsigned int vertex_stride = 13;

	for (unsigned int i = pos_offset; i < vertices_count; i += vertex_stride)
		UpdateAABB(glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]));


	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	VAO.Generate();

	VBO = VertexBuffer(vertices, sizeof(vertices));
	IBO = IndexBuffer(indices, 36);
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
