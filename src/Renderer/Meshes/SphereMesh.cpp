#include "SphereMesh.h"
#include "Util/MathsHelpers.h"

#include "../RendererErrorAssertion.h"
#include "../ObjectBuffer/VertexBufferLayout.h"

#include <iostream>

SphereMesh::SphereMesh()
{
	SectorCount = m_SectorCount;
	SpanCount = m_SpanCount;
}

void SphereMesh::Create()
{
	m_Vertices.clear();
	m_Indices.clear();
	//vextex position
	float x, y, z, w;
	//vectex texture coord
	float u, v;

	float sector_angle;
	float span_angle;

	//TO-DO: for now use float, double will be too redundant to use 
	float sector_step = (float)(2 * MathsHelper::PI / m_SectorCount);  // 0 - 360(2pi)/count =>  angle btw each steps
	float span_step = (float)(MathsHelper::PI / m_SpanCount);		  // 0 - 180(pi)/count => angle btw each step

	float radius = 1.0f;

	//compute & store vertices
	for (unsigned int i = 0; i <= m_SpanCount; i++)
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

		w = radius * sinf(span_angle);
		y = radius * cosf(span_angle);

		//add (sector_count + 1) vertices per stack
		//first and last vertices have same position, but different tex coords 
		for (unsigned int j = 0; j <= m_SectorCount; ++j)
		{
			//start from 0 >> current step * step angle >> 360 
			sector_angle = j * sector_step;

			//vectex position (x, y, z)
			x = w * cosf(sector_angle);			//r * cos(u) * cos(v)
			z = w * sinf(sector_angle);			//r * cos(u) * sin(v)
			m_Vertices.push_back(x);
			m_Vertices.push_back(y);
			m_Vertices.push_back(z);
			UpdateAABB(glm::vec3(x, y, z));
			m_Vertices.push_back(1.0f);    //w
			//m_Vertices.push_back(w);    //w

			//vextex colour 
			m_Vertices.push_back(x);		//r	
			m_Vertices.push_back(y);		//g	
			m_Vertices.push_back(z);		//b	
			m_Vertices.push_back(1.0f);		//a	

			//vertex texture coord range between [0, 1]
			u = (float)j / m_SectorCount;
			v = (float)i / m_SpanCount;
			m_Vertices.push_back(u);
			m_Vertices.push_back(v);

			float nx = x, ny = y, nz = z;

			m_Vertices.push_back(nx);
			m_Vertices.push_back(ny);
			m_Vertices.push_back(nz);
		}
	}

	//compute & store indices
	unsigned int k1, k2;
	for (unsigned int i = 0; i < m_SpanCount; ++i)
	{
		k1 = i * (m_SectorCount + 1);		//beginning of current stack
		k2 = k1 + m_SectorCount + 1;			//beginning of next stack

		for (unsigned int j = 0; j < m_SectorCount; ++j, ++k1, ++k2)
		{
			//2 triangles per sector excluding first and last stacks
			//k1 => k2 => k1+1
			if (i != 0)
			{
				m_Indices.push_back(k1);
				m_Indices.push_back(k2);
				m_Indices.push_back(k1 + 1);
			}

			//k1+1 => k2 => k2+ 1
			if (i != (m_SpanCount - 1))
			{
				m_Indices.push_back(k1 + 1);
				m_Indices.push_back(k2);
				m_Indices.push_back(k2 + 1);
			}

		}
	}

	//ReCalcNormalsWcIndices(&m_Vertices[0], &m_Indices[0], m_Vertices.size(), m_Indices.size(), 13, 10);
	CacheVertices(&m_Vertices[0], m_Vertices.size());


	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	VAO.Generate();

	VBO = VertexBuffer(&m_Vertices[0], sizeof(m_Vertices[0]) * m_Vertices.size());

	IBO = IndexBuffer(&m_Indices[0], m_Indices.size());
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

void SphereMesh::Create(unsigned int segment)
{
	Clear();
	m_SectorCount = segment;
	m_SpanCount = segment / 2;
	Create();
}

void SphereMesh::Update()
{
	if (SectorCount != m_SectorCount || SpanCount != m_SpanCount)
	{
		m_SectorCount = SectorCount;
		m_SpanCount = SpanCount;
		Clear();
		Create();
	}
}
