#include "Skybox.h"

#include "Camera.h";
#include "Window.h"

Skybox::Skybox() : m_FacePath(std::vector<std::string>())
{}

Skybox::Skybox(const std::vector<std::string> faces_image_path)
{
	m_FacePath = faces_image_path;
	Create(m_FacePath);
}

void Skybox::Create(const std::vector<std::string> faces_image_path)
{
	m_FacePath = faces_image_path;
	m_TextureMap.LoadTexture(faces_image_path);

	ShaderFilePath skybox_shader_file_path
	{ "src/ShaderFiles/SkyboxVertex.glsl",
		//"src/ShaderFiles/Learning/ScreenFrameFrag.glsl" };
		"src/ShaderFiles/SkyboxFragment.glsl" };
	m_Shader.Create("skybox_shader", skybox_shader_file_path);

	m_Mesh.Create();
}

void Skybox::Draw(Camera& camera, Window& window) 
{
	m_Shader.Bind();
	glDepthMask(GL_FALSE);

	glm::mat4 sky_view = glm::mat4(glm::mat3(camera.CalViewMat()));
	m_Shader.SetUniformMat4f("u_View", sky_view);
	m_Shader.SetUniformMat4f("u_Projection", camera.CalculateProjMatrix(window.GetAspectRatio()));

	m_TextureMap.Activate();

	m_Mesh.Render();
	//m_Mesh.GetVAO()->Bind();
	//glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	//m_TextureMap.DisActivate();
	//m_Mesh.GetVAO()->Unbind();
	m_Shader.UnBind();
}

void Skybox::Destroy()
{
	m_TextureMap.UnRegisterUse();
}