#include "PrimitiveMeshFactory.h"

#include "glm/glm.hpp"
#include "Mesh.h"

#include "Util/MathsHelpers.h"

namespace CRRT
{
	std::shared_ptr<Mesh> PrimitiveMeshFactory::CreateQuad()
	{
		
		////For cal Tangent & Bitagent
		glm::vec3 p0(-0.5f, 0.5f, 0.0f),
				p1(-0.5f, -0.5f, 0.0f),
				p2(0.5f, -0.5f, 0.0f), 
				p3(0.5f, 0.5f, 0.0f);

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
			/*0*/	p0.x, p0.y, p0.z, 1.0f,		1.0f, 0.0f, 1.0f, 1.0f,  uv0.x, uv0.y,   nor.x, nor.y, nor.z,  t0.x, t0.y, t0.z,  bt0.x, bt0.y, bt0.z,
			/*1*/	p1.x, p1.y, p1.z, 1.0f,		1.0f, 0.0f, 1.0f, 1.0f,  uv1.x, uv1.y,   nor.x, nor.y, nor.z,  t0.x, t0.y, t0.z,  bt0.x, bt0.y, bt0.z,
			/*2*/	p2.x, p2.y, p2.z, 1.0f,		1.0f, 0.0f, 1.0f, 1.0f,  uv2.x, uv2.y,   nor.x, nor.y, nor.z,  t0.x, t0.y, t0.z,  bt0.x, bt0.y, bt0.z,
															
															
			/*3*/	p0.x, p0.y, p0.z, 1.0f,		1.0f, 0.0f, 1.0f, 1.0f,  uv0.x, uv0.y,   nor.x, nor.y, nor.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
			/*4*/	p2.x, p2.y, p2.z, 1.0f,		1.0f, 0.0f, 1.0f, 1.0f,  uv2.x, uv2.y,   nor.x, nor.y, nor.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
			/*5*/	p3.x, p3.y, p3.z, 1.0f,		1.0f, 0.0f, 1.0f, 1.0f,  uv3.x, uv3.y,   nor.x, nor.y, nor.z,  t1.x, t1.y, t1.z,  bt1.x, bt1.y, bt1.z,
		};


		//0, 1, 2, 3, 4, 5 >> 0 1 2 0 2 3
		unsigned int newIndices[] =
		{
			0, 1, 2,
			3, 4, 5

			//3, 2, 0
		};


		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indices;

		ConvertVerticesArrayToVector(newVertices, sizeof(newVertices) / sizeof(newVertices[0]), 19, _vertices);

		for (auto& i : newIndices)
			_indices.push_back(i);

		return std::make_shared<Mesh>(_vertices, _indices);
	}

	std::shared_ptr<Mesh> PrimitiveMeshFactory::CreateSphere(unsigned int sector_count, unsigned int span_count)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		//vextex position
		float x, y, z, w;
		//vectex texture coord
		float u, v;

		float sector_angle;
		float span_angle;

		//TO-DO: for now use float, double will be too redundant to use 
		float sector_step = (float)(2 * MathsHelper::PI / sector_count);  // 0 - 360(2pi)/count =>  angle btw each steps
		float span_step = (float)(MathsHelper::PI / span_count);		  // 0 - 180(pi)/count => angle btw each step

		//float radius = 1.0f;
		float radius = 0.5f;

		//compute & store vertices
		for (unsigned int i = 0; i <= span_count; i++)
		{
			// 180 degree(pi) to 0 degree //0 degree to 180 degree(pi)
			span_angle = (float)MathsHelper::PI - i * span_step;

			//parametric equation for sphere
			// x = center_x + r * sin(theta) * cos(phi)    
			// y = center_y + r * sin(thata) * sin(phi)
			// z = center_z + r * cos(theta)
			// where 
			//		theta = span_angle range 0 to pi(180 degrees)
			//		phi = sector_angle range 0 to 2pi(360 degrees)
			//RETERIVED: https://en.m.wikipedia.org/wiki/Sphere
			//			In their case z is up axis

		
			w = radius * glm::sin(span_angle);
			y = radius * glm::cos(span_angle);

			//add (sector_count + 1) vertices per stack
			//first and last vertices have same position, but different tex coords 
			for (unsigned int j = 0; j <= sector_count; ++j)
			{
				//start from 0 >> current step * step angle >> 360 
				sector_angle = j * sector_step;

				//vectex position (x, y, z)
				x = w * glm::cos(sector_angle);			//r * cos(u) * cos(v)
				z = w * glm::sin(sector_angle);			//r * cos(u) * sin(v)

				//vertex texture coord range between [0, 1]
				u = (float)j / sector_count;
				v = (float)i / span_count;

				Vertex vertex
				{
					{x,y, z, /*w*/1.0f},    //layout 0 => pos
					{1.0f, 0.0f, 1.0f, 1.0f},	//layout 1 => col  //magenta for debugging
					{u, v},					   //layout 2 => uv
					{x, y, z},				  //layout 3 => nor
					{0.0f, 0.0f, 0.0f},       //layout 4 => tan
					{0.0f, 0.0f, 0.0f},       //layout 5 => bi tan
				};
				vertices.push_back(vertex);
			}
		}

		
		//compute & store indices
		unsigned int k1, k2;
		for (unsigned int i = 0; i < span_count; ++i)
		{
			k1 = i * (sector_count + 1);		//beginning of current stack
			k2 = k1 + sector_count + 1;			//beginning of next stack

			for (unsigned int j = 0; j < sector_count; ++j, ++k1, ++k2)
			{
				//2 triangles per sector excluding first and last stacks
				//k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				//k1+1 => k2 => k2+ 1
				if (i != (span_count - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}

			}
		}

		return std::make_shared<Mesh>(vertices, indices);
	}

	std::shared_ptr<Mesh> PrimitiveMeshFactory::CreateCube()
	{
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


		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indices;


		ConvertVerticesArrayToVector(vertices, sizeof(vertices) / sizeof(vertices[0]), /*19*/13, _vertices);

		for (auto& i : indices)
			_indices.push_back(i);

		return std::make_shared<Mesh>(_vertices, _indices);
	}


	/// <summary>
	/// Not ideally but works for now
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <param name="vertices"></param>
	void PrimitiveMeshFactory::ConvertVerticesArrayToVector(float* data, unsigned int size, unsigned int offset, std::vector<Vertex>& vertices)
	{
		Vertex temp;
		for (unsigned int v_pointer = 0; v_pointer < size; v_pointer += offset)
		{
			
			temp.position[0] = data[v_pointer];
			temp.position[1] = data[v_pointer + 1];
			temp.position[2] = data[v_pointer + 2];
			temp.position[3] = data[v_pointer + 3];

			temp.colour[0] = data[v_pointer + 4];  //(0+3+1) offset
			temp.colour[1] = data[v_pointer + 4 + 1];
			temp.colour[2] = data[v_pointer + 4 + 2];
			temp.colour[3] = data[v_pointer + 4 + 3];

			temp.texCoord[0] = data[v_pointer + 8]; //(4+3+1) offset
			temp.texCoord[1] = data[v_pointer + 8 + 1];

			temp.normals[0] = data[v_pointer + 10];  //(8+1+1) offset
			temp.normals[1] = data[v_pointer + 10 + 1];
			temp.normals[2] = data[v_pointer + 10 + 2];

			//Safety for mesh that does not have tangents 
			//if (offset < 13)
			//	continue;

			temp.tangent[0] = data[v_pointer + 13];  //(10+2+1) offset
			temp.tangent[1] = data[v_pointer + 13 + 1];
			temp.tangent[2] = data[v_pointer + 13 + 2];

			temp.bitangent[0] = data[v_pointer + 16];  //(13+2+1) offset
			temp.bitangent[1] = data[v_pointer + 16 + 1];
			temp.bitangent[2] = data[v_pointer + 16 + 2];
			vertices.push_back(temp);
		}
	}

}
