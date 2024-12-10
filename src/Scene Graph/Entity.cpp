#include "Entity.h"

#include "Renderer/Shader.h"
#include "Renderer/Material.h"
#include "Renderer/Meshes/Mesh.h"

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

	//override for now (for renderable mesh
	UpdateWorldTransform();

	for (auto& c : m_Children)
		c->MarkTransformDirty();
}

void Entity::ConstructAABB()
{
	if(m_Mesh)
		m_AABB = m_Mesh->GetAABB();
}

const glm::mat4& Entity::GetWorldTransform()
{
	if (m_DirtyTranform)
		UpdateWorldTransform();

	return m_WorldTransform;
}

glm::mat4* Entity::GetWorldTransformPtr()
{
	if (m_DirtyTranform)
		UpdateWorldTransform();

	return &m_WorldTransform;
}

const AABB Entity::GetEncapsulatedChildrenAABB()
{
	AABB bounds = AABB(GetWorldTransform()[3]);

	if (m_Mesh)
	{
		//get entity mesh AABB
		bounds = m_Mesh->GetAABB();
		//transform aabb, based on entity transformation
		bounds = bounds.Tranformed(GetWorldTransform());
	}
	//Recursive encapsulate children AABB
	for (const auto& c : m_Children)
		bounds.Encapsulate(c->GetEncapsulatedChildrenAABB());

	return bounds;
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
