#include "GameObject.h"
#include <iostream>

#include "Renderer/Meshes/Mesh.h"
#include "Renderer/Texture.h"

//////////////////////////////////////////////
//Deprecated Class, Entity is Object class now
//////////////////////////////////////////////

GameObject::GameObject(Mesh* mesh, Texture* texture, bool a_sphere)
	: m_Mesh(mesh), m_Texture(texture), aSphere(a_sphere)
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
	m_SceneRenderer.DrawMesh(*m_Mesh);
}

void GameObject::DebugDraw()
{
	m_SceneRenderer.DrawMeshOutline(*m_Mesh);
}

void GameObject::Cleanup()
{
	std::cout << "Warning Warning !!!!!!! Game cleanup is called\n";
	//This is crazyy but yeah it worked
	//lesson if an obj/class pointer is not pointing to anything "nullptr"
	//any variable/some symbols will be able to read 
	//has not heap memeory has been allocated for them
	//but already fine with "delete" keyword
	if (!this)
		return;

	if(m_Mesh)
		m_Mesh->UnRegisterUse();
	if(m_Texture)
		m_Texture->UnRegisterUse();
	//TO-DO: Later set up ref count for Mesh&Texture
	//       As so if no GaemObject is ref them it should get deleted
	m_Mesh = nullptr;
	m_Texture = nullptr;
	//m_Material = nullptr;
}



