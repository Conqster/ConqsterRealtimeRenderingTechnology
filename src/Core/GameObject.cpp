#include "GameObject.h"
#include <iostream>

#include "Graphics/Meshes/Mesh.h"
#include "Graphics/Texture.h"

#include "Graphics/Material.h"

GameObject::GameObject(Mesh* mesh, Texture* texture)
	: m_Mesh(mesh), m_Texture(texture)
{
	m_Mesh->RegisterUse();
	m_Texture->RegisterUse();
	std::cout << "GameObject constructed !!!!!!!!!!!!!!!!!\n";
}

GameObject::~GameObject()
{
	//m_Mesh.Clear();
	Cleanup();
	std::cout << "Cleared gameObject\n";
}

void GameObject::Draw()
{
	m_Texture->Activate();
	//m_Material->Use();
	m_Mesh->Render();
	//m_Texture->DisActivate();
	//m_Mesh.Render();
}

void GameObject::DebugDraw()
{
	m_Mesh->RenderDebugOutLine();
	//m_Mesh.RenderDebugOutLine();
}

void GameObject::Cleanup()
{
	//m_Mesh.Clear();
	if(m_Mesh)
		m_Mesh->UnRegisterUse();
	if(m_Texture)
		m_Texture->UnRegisterUse();
	//TO-DO: Later set up ref count for Mesh&Texture
	//       As so if no GaemObject is ref them it should get deleted
	m_Mesh = nullptr;
	m_Texture = nullptr;

}



