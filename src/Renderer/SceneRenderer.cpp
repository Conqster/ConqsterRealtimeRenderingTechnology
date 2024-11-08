#include "SceneRenderer.h"

#include "Meshes/Mesh.h"
#include "RendererErrorAssertion.h"

namespace CRRT
{
	void SceneRenderer::Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		glViewport(x, y, width, height);
	}
	void SceneRenderer::DrawMesh(const std::shared_ptr<Mesh>& mesh)
	{
		auto vao = mesh->GetVAO();
		auto ibo = mesh->GetIBO();
		vao->Bind();
		ibo->Bind();
		GLCall(glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, (void*)0));
	}
	void SceneRenderer::DrawMesh(Mesh& mesh)
	{
		auto vao = mesh.GetVAO();
		auto ibo = mesh.GetIBO();
		vao->Bind();
		ibo->Bind();
		GLCall(glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, (void*)0));
	}
	void SceneRenderer::DrawMeshOutline(Mesh& mesh, float thickness)
	{
		auto vao = mesh.GetVAO();
		auto ibo = mesh.GetIBO();
		vao->Bind();
		ibo->Bind();
		GLCall(glLineWidth(thickness));
		GLCall(glDrawElements(GL_LINE_LOOP, ibo->GetCount(), GL_UNSIGNED_INT, (void*)0));
	}
	void SceneRenderer::DrawMeshInstance(const std::shared_ptr<Mesh>& mesh, int count)
	{
		auto vao = mesh->GetVAO();
		auto ibo = mesh->GetIBO();
		vao->Bind();
		ibo->Bind();
		GLCall(glDrawElementsInstanced(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, (void*)0, count));
	}
	void SceneRenderer::DrawMeshInstance(Mesh& mesh, int count)
	{
		auto vao = mesh.GetVAO();
		auto ibo = mesh.GetIBO();
		vao->Bind();
		ibo->Bind();
		GLCall(glDrawElementsInstanced(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, (void*)0, count));
	}
}

