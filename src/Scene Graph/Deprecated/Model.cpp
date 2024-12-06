#include "Model.h"
//////////////////////////////////////////////
//Deprecated Class, Entity is Object class now
//////////////////////////////////////////////

Model::Model(std::vector<ModelMesh> _meshes)
{
	meshes = _meshes;

	aabb = AABB(meshes[0].GetAABB());
	for (auto& m : meshes)
		UpdateAABB(m.GetAABB());
}

void Model::Draw()
{
	for (auto& mesh : meshes)
		m_SceneRenderer.DrawVertexElements(mesh.GetVAO(), mesh.GetIBO());
}

void Model::Draw(Shader& shader)
{
	for (auto& mesh : meshes)
		mesh.NewRender(shader);
}

void Model::DebugWireDraw()
{
	for (auto& mesh : meshes)
		m_SceneRenderer.DrawVertexElementsOutline(mesh.GetVAO(), mesh.GetIBO());
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
