#include "Scene Graph/Model.h"

Model::Model(std::vector<ModelMesh> _meshes)
{
	meshes = _meshes;

	aabb = AABB(meshes[0].GetAABB());
	for (auto& m : meshes)
		UpdateAABB(m.GetAABB());



	//i could generate mesh data here by loop through all the meshes
	//for(auto& mesh : meshes)
	//	mesh......
}

void Model::Draw()
{
	for (auto& mesh : meshes)
		m_SceneRenderer.DrawMesh(mesh);
		//mesh.Render();
}

void Model::Draw(Shader& shader)
{
	for (auto& mesh : meshes)
		mesh.NewRender(shader);
}

void Model::DebugWireDraw()
{
	for (auto& mesh : meshes)
		m_SceneRenderer.DrawMeshOutline(mesh);
		//mesh.RenderDebugOutLine();
}

void Model::Destroy()
{
	for (auto& m : meshes)
		m.Clear();

	for (auto& t : textures)
		t.UnRegisterUse();
		
}

void Model::UpdateAABB(const AABB &inRHS)
{
	aabb.Encapsulate(inRHS);
}

void Model::RecalculateAABBFromMesh()
{
	aabb = AABB(meshes[0].RecalculateAABB());
	for (auto& m : meshes)
		aabb.Encapsulate(m.RecalculateAABB());  //need to fix later not to redo first mesh again

}
