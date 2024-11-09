#include "Entity.h"

#include "Renderer/Shader.h"
#include "Renderer/Material.h"
#include "Renderer/Meshes/Meshes.h"

void Entity::UpdateWorldTransform()
{
	if (m_Parent)//auto transverse the parent child link
		m_WorldTransform = m_Parent->GetWorldTransform() * m_LocalTransform;
	else
		m_WorldTransform = m_LocalTransform;

	m_DirtyTranform = false;
}

void Entity::MarkTransformDirty()
{
	m_DirtyTranform = true;
	for (auto& c : m_Children)
		c->MarkTransformDirty();
}

const glm::mat4& Entity::GetWorldTransform()
{
	if (m_DirtyTranform)
		UpdateWorldTransform();

	return m_WorldTransform;
}

void Entity::AddLocalChild(const Entity& entity)
{
	m_Children.emplace_back(std::make_shared<Entity>(entity));
	m_Children.back()->m_Parent = GetRef();
	m_Children.back()->MarkTransformDirty();
}

void Entity::AddWorldChild(const Entity& entity)
{
	m_Children.emplace_back(std::make_shared<Entity>(entity));
	m_Children.back()->m_Parent = GetRef();
	m_Children.back()->m_LocalTransform = glm::inverse(m_WorldTransform) * m_Children.back()->m_WorldTransform;
	m_Children.back()->MarkTransformDirty();
}

void Entity::Draw(Shader& shader)
{
	if (m_Children.size() > 0)
		for (auto& e : m_Children)
			e->Draw(shader);

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
		if (m_Material->normalMap)
		{
			m_Material->normalMap->Activate(tex_units);
			shader.SetUniform1i("u_Material.normalMap", tex_units++);
		}
		if (m_Material->parallaxMap)
		{
			m_Material->parallaxMap->Activate(tex_units);
			shader.SetUniform1i("u_Material.parallaxMap", tex_units++);
		}
		shader.SetUniform1i("u_Material.useNormal", m_Material->useNormal && m_Material->normalMap);
		shader.SetUniform1i("u_Material.shinness", m_Material->shinness);
		shader.SetUniform1i("u_Material.useParallax", m_Material->useParallax);
		shader.SetUniform1f("u_Material.parallax", m_Material->heightScale);
	}


	//shader.SetUniformMat4f("u_Model", m_Transform);
	//use GetWorldTransform as it might have a parent
	shader.SetUniformMat4f("u_Model", GetWorldTransform());//expensive but works for now

	//m_Mesh->Render();
	//m_SceneRenderer.DrawMesh(*m_Mesh);

	//would have to do this as most model would have a default/
	//mesh that has no material would be group has no shading but debug pink
	//if (m_Material)
		//m_Material->baseMap->DisActivate();
}

void Entity::Destroy()
{
	//resource manager could take care of the mesh & material as its shader



	//for (auto& t : textures)
	//	t.UnRegisterUse();
}

void Entity::AddLocalChild(const std::shared_ptr<Entity>& entity)
{
	m_Children.emplace_back(entity);
	m_Children.back()->m_Parent = GetRef();
	m_Children.back()->MarkTransformDirty();
}

void Entity::AddWorldChild(const std::shared_ptr<Entity>& entity)
{
	m_Children.emplace_back(entity);
	m_Children.back()->m_Parent = GetRef();
	m_Children.back()->m_LocalTransform = glm::inverse(m_WorldTransform) * m_Children.back()->m_WorldTransform;
	m_Children.back()->MarkTransformDirty();
}

void Entity::UpdateViewSqrDist(const glm::vec3& view_pos)
{
	//m_SqrViewDist = glm::distance2(view_pos, GetWorldTransform()[3]);
	glm::vec3 pos = GetWorldTransform()[3];
	m_SqrViewDist = glm::distance(view_pos, pos);
}
