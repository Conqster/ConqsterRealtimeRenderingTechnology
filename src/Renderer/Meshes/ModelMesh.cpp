#include "ModelMesh.h"

#include "../RendererErrorAssertion.h"
#include "../ObjectBuffer/VertexBufferLayout.h"

#include "../Texture.h"
//#include "../Shader.h"

#include "../Material.h"

void ModelMesh::Create()
{
}

void ModelMesh::Generate(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	m_Vertices = vertices;
	//to recenter AABB as mesh origin/center is not 0,0,0
	aabb = AABB(glm::vec3(vertices[0].position[0], vertices[0].position[1], vertices[0].position[2]));
	for(auto& v : vertices)
		UpdateAABB(glm::vec3(v.position[0], v.position[1], v.position[2]));


	m_Textures = textures;
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
	VAO.AddBufferLayout(VBO, vbLayout);
}

void ModelMesh::NewRender(Shader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int emissiveNr = 1;


	//for (unsigned int i = 0; i < m_Textures.size(); i++)
	//{
	//	std::string num;
	//	//std::string name = TypeToString(m_Textures[i].

	//	//std::string name = TypeToString(m_Textures[i].GetType());

	//	//if (name == "texture_diffuse")
	//	//	num = std::to_string(diffuseNr++);
	//	//else if (name == "texture_specular")
	//	//	num = std::to_string(specularNr++);
	//	//else if (name == "texture_emissive")
	//	//	num = std::to_string(emissiveNr++);

	//	//shader.SetUniform1i(("u_Material." + name + num).c_str(), i);
	//	m_Textures[i].Activate();
	//}

	if(m_Textures.size() > 0)
		m_Textures[0].Activate();

	VAO.Bind();
	IBO.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, IBO.GetCount(), GL_UNSIGNED_INT, (void*)0));

	VAO.Unbind();
	IBO.Unbind();

	//for (unsigned int i = 0; i < m_Textures.size(); i++)
	//	m_Textures[i].DisActivate();
}

AABB ModelMesh::RecalculateAABB()
{
	glm::vec3 center = glm::vec3(0.0f);
	std::vector<glm::vec3> vp;
	glm::vec3 low = glm::vec3(0.0f);
	glm::vec3 high = glm::vec3(0.0f);
	//sum up the mesh vertices pos
	for (auto& v : m_Vertices)
	{
		glm::vec3 p = glm::vec3(v.position[0], v.position[1], v.position[2]);
		low = (glm::min)(low, p);
		high = (glm::max)(high, p);
		//p *= 10.0f;
		vp.push_back(p);
		//center += p;
		center += glm::normalize(p);
	}
	printf("Low vec x: %f, y: %f,z: %f\n", low.x, low.y, low.z);
	printf("High vec x: %f, y: %f, z: %f\n", high.x, high.y, high.z);
	printf("Low vec mag: %f\n", glm::length(low));
	printf("High vec mag: %f\n", glm::length(high));
	printf("average mag: %f\n",((glm::length(low) + glm::length(high)) * 0.5f));
	low = glm::normalize(low);
	high = glm::normalize(high);
	printf("Low vec dir x: %f, y: %f,z: %f\n", low.x, low.y, low.z);
	printf("High vec dir x: %f, y: %f,z: %f\n", high.x, high.y, high.z);
	//get average 
	center /= vp.size();
	//set aabb min, max to center i.e its center is the mesh cenetr
	aabb = AABB(center);
	//encapsulate all the vertex positions in the bounds
	for (auto& v : vp)
		aabb.Encapsulate(v + center);

	return aabb;
}

void ModelMesh::Generate(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::shared_ptr<Material> mat)
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
	VAO.AddBufferLayout(VBO, vbLayout);
}



