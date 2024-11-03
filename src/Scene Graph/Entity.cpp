#include "Entity.h"

#include "Renderer/Shader.h"
#include "Renderer/Material.h"
#include "Renderer/Meshes/Meshes.h"

void Entity::Draw(Shader& shader)
{
	if (!m_Mesh)
		return;


	shader.Bind();
	//need to take this out later, because the shader/shaders shader program only needs
	//to bind the material once per material as multiple entity could use the same material
	if (m_Material)
	{
		unsigned int tex_units = 0;
		shader.SetUniformVec3("u_Material.baseColour", m_Material->baseColour);
		if (m_Material->baseMap)
		{
			m_Material->baseMap->Activate(tex_units);
			shader.SetUniform1i("u_Material.baseMap", tex_units++);
		}
		bool has_nor = (m_Material->normalMap) ? true : false;
		if (has_nor)
		{
			m_Material->normalMap->Activate(tex_units);
			shader.SetUniform1i("u_Material.normalMap", tex_units++);
		}
		if (m_Material->parallaxMap)
		{
			m_Material->parallaxMap->Activate(tex_units);
			shader.SetUniform1i("u_Material.parallaxMap", tex_units++);
		}
		shader.SetUniform1i("u_UseNorMap", has_nor);
		shader.SetUniform1i("u_Material.shinness", m_Material->shinness);
		shader.SetUniform1i("u_Material.useParallax", m_Material->useParallax);
		shader.SetUniform1f("u_Material.parallax", m_Material->heightScale);
	}


	shader.SetUniformMat4f("u_Model", m_Transform);
	m_Mesh->Render();
}

void Entity::Destroy()
{
	//resource manager could take care of the mesh & material as its shader



	//for (auto& t : textures)
	//	t.UnRegisterUse();
}
