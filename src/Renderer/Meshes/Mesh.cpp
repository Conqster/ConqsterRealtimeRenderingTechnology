#include "Mesh.h"
#include "GL/glew.h"
#include "../ObjectBuffer/VertexBufferLayout.h"

//TO-DO: might change/move GLCall helper
//       to a helper class from Renderer 
//		 avoid circle include
#include "../RendererErrorAssertion.h"
#include <iostream>

#include "Util/MathsHelpers.h"

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

void Mesh::ReCalcNormalsWcIndices(float* vertices, unsigned int* indices, unsigned int vertices_count, unsigned int indices_count, unsigned int vertex_stride, unsigned int normal_offset, bool reset_normals)
{
	//return;
	//(Quick hack): set all normals to zero
	if (reset_normals)
	{
		for (unsigned int i = normal_offset; i < vertices_count; i += vertex_stride)
		{
			vertices[i] = vertices[i + 1] = vertices[i + 2] = 0.0f;
		}
	}


	for (size_t i = 0; i < indices_count; i += 3)
	{
		//base on the idx, get the first (pos-x) of the current indices i, i+1, i+2 (3 indices makes triangle/face)
		unsigned int v_in0 = indices[i] * vertex_stride;     //pointer tracker -> pos_x of vertex of indice i (v_in0)
		unsigned int v_in1 = indices[i + 1] * vertex_stride; //pointer tracker -> next == pos_x of vertex of indice i+1  (v_in1)
		unsigned int v_in2 = indices[i + 2] * vertex_stride; //pointer tracker -> next == pos_x of vertex of indice i+2  (v_in2)

		glm::vec3 v01;  //vec v_in0 to v_in1 i.e v_in1 - v_in0
		glm::vec3 v02; //vec v_in0 to v_in2
		//Luxury that all the array data in vertex are packed contagoiusly 
		v01 = glm::vec3(vertices[v_in1] - vertices[v_in0],         //v_in1.x - v_in0.x
			vertices[v_in1 + 1] - vertices[v_in0 + 1],     //v_in1.y - v_in0.y
			vertices[v_in1 + 2] - vertices[v_in0 + 2]);    //v_in1.z - v_in0.z

		v02 = glm::vec3(vertices[v_in2] - vertices[v_in0],         //v_in2.x - v_in0.x
			vertices[v_in2 + 1] - vertices[v_in0 + 1],     //v_in2.y - v_in0.y
			vertices[v_in2 + 2] - vertices[v_in0 + 2]);    //v_in2.z - v_in0.z


		//normals for all 3 current vertices (i.e all contribute to make a face)
		/////////v_in0//////// v01 = v_in1 - v_in0
		////////////#///////// v02 = v_in0 to v_in2
		//////////#//#//////// n for F = Cross(v01, v02)
		/////////#////#///////
		////////#//////#//////
		///////#///F////#/////
		//////#//////////#////
		////##############////
		//v_in1//////////v_in2
		glm::vec3 nor = glm::cross(v01, v02);
		nor = glm::normalize(nor);

		//accumuate normals of all current vertices as other faces indice might also contribute
		v_in0 += normal_offset;  //move pointer tracker -> from pos_x to nor_x of vertex of indice i
		v_in1 += normal_offset;  //move pointer tracker -> from pos_x to nor_x of vertex of indice i+1
		v_in2 += normal_offset;  //move pointer tracker -> from pos_x to nor_x of vertex of indice i+2

		//Smooth shading
		vertices[v_in0] += nor.x;
		vertices[v_in0 + 1] += nor.y;
		vertices[v_in0 + 2] += nor.z;

		vertices[v_in1] += nor.x;
		vertices[v_in1 + 1] += nor.y;
		vertices[v_in1 + 2] += nor.z;

		vertices[v_in2] += nor.x;
		vertices[v_in2 + 1] += nor.y;
		vertices[v_in2 + 2] += nor.z;

		//looks Flat shaded
		//vertices[v_in0] = nor.x;
		//vertices[v_in0 + 1] = nor.y;
		//vertices[v_in0 + 2] = nor.z;

		//vertices[v_in1] = nor.x;
		//vertices[v_in1 + 1] = nor.y;
		//vertices[v_in1 + 2] = nor.z;

		//vertices[v_in2] = nor.x;
		//vertices[v_in2 + 1] = nor.y;
		//vertices[v_in2 + 2] = nor.z;
	}

	//normalize all acculated normals for each vertex
	for (unsigned int i = normal_offset; i < vertices_count; i += vertex_stride)
	{
		glm::vec3 acc_nor(vertices[i],
			vertices[i + 1],
			vertices[i + 2]);

		if (glm::length(acc_nor) > 0.0f)
			acc_nor = glm::normalize(acc_nor);

		vertices[i] = acc_nor.x;
		vertices[i + 1] = acc_nor.y;
		vertices[i + 2] = acc_nor.z;
	}
}



void Mesh::Generate(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	m_Vertices = vertices;
	//to recenter AABB as mesh origin/center is not 0,0,0
	aabb = AABB(glm::vec3(vertices[0].position[0], vertices[0].position[1], vertices[0].position[2]));
	for (auto& v : vertices)
		UpdateAABB(glm::vec3(v.position[0], v.position[1], v.position[2]));


	//m_Textures.emplace_back(mat->baseMap);
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	VAO.Generate();

	VBO = VertexBuffer(&vertices[0], sizeof(vertices[0]) * vertices.size());
	IBO = IndexBuffer(&indices[0], indices.size());
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
	//tangent btx, bty, btz
	vbLayout.Push<float>(3);
	VAO.AddBufferLayout(VBO, vbLayout);
}

Mesh::~Mesh()
{
	//Clear();
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

void Mesh::UpdateAABB(glm::vec3 v)
{
	aabb.Encapsulate(v);
}

