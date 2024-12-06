#include "Skybox.h"

#include "Camera.h"
#include "Window.h"

#include "RendererErrorAssertion.h"

#include "RenderCommand.h"
#include "SceneRenderer.h"

#include "Meshes/PrimitiveMeshFactory.h"


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

	ShaderFilePath skybox_shader_file_path{ 
		"Assets/Shaders/Utilities/Skybox/SkyboxVertex(deprecated).glsl",
		"Assets/Shaders/Utilities/Skybox/SkyboxFragment.glsl" };

	m_Shader.Create("default_skybox_shader", skybox_shader_file_path);

	m_Mesh = CRRT::PrimitiveMeshFactory::Instance().CreateACube();
}


/// <summary>
/// when/if using uniform buffer to store camera matrix, it not necssary to pass 
/// the camera & window into the skybox draw function. their may purpose is to do the 
/// view & projection matrix calculation. 
/// </summary>
/// <param name="camera"></param>
/// <param name="window"></param>
void Skybox::Draw(Camera& camera, Window& window) 
{
	m_Shader.Bind();
	glCullFace(GL_FRONT);
	GLCall(glDepthMask(GL_FALSE));

	glm::mat4 sky_view = glm::mat4(glm::mat3(camera.CalViewMat()));
	m_Shader.SetUniformMat4f("u_View", sky_view);
	m_Shader.SetUniformMat4f("u_Projection", camera.CalculateProjMatrix(window.GetAspectRatio()));
	m_TextureMap.Activate();
	m_SceneRenderer.DrawMesh(m_Mesh);
	GLCall(glDepthMask(GL_TRUE));
	m_TextureMap.DisActivate();
	m_Shader.UnBind();
	glCullFace(GL_BACK);
}

void Skybox::Draw(Shader& shader, CRRT::SceneRenderer& renderer)
{
	shader.Bind();
	RenderCommand::CullFront();
	RenderCommand::DepthMask(false);
	m_TextureMap.Activate();
	renderer.DrawMesh(m_Mesh);
	RenderCommand::DepthMask(true);
	m_TextureMap.DisActivate();
	shader.UnBind();
	RenderCommand::CullBack();
}

void Skybox::ActivateMap(uint16_t slot)
{
	m_TextureMap.Activate(slot);
}

void Skybox::DisActivate()
{
	m_TextureMap.DisActivate();
}

void Skybox::Destroy()
{
	m_TextureMap.DisActivate();
	m_TextureMap.UnRegisterUse();
}
