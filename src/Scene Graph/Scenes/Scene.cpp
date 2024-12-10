#pragma once
#include "Scene.h"
#include "Renderer/Renderer.h"

#include "Util/FilePaths.h"

#include "Scene Graph/Entity.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Material.h"
#include "Renderer/Shader.h"

#include "Renderer/Meshes/PrimitiveMeshFactory.h"

void Scene::SetWindow(Window* window)
{
	this->window = window;
}


void Scene::OnInit(Window* window)
{
	this->window = window;
	GLCall(glViewport(0, 0, window->GetWidth(), window->GetHeight()));

	blank_tex = new Texture(FilePaths::Instance().GetPath("blank-image"));
}


void Scene::OnUpdate(float delta_time)
{

}


void Scene::OnRender()
{
	//RenderCommand::ClearColour(m_ClearScreenColour);
	//RenderCommand::Clear();

	//PreUpdateGPUUniformBuffers(*m_Camera);

	////Build Scene
	//
	////ShadowPass()

	////IntermidateUpdateGPUUniformBuffers(dir light, pt lights, max light)

	//switch (m_RenderingPath)
	//{
	//case Forward:
	//	ForwardShading(def_OpaqueEntities, def_TransparentEntities);
	//	break;
	//case Deferred:
	//	DeferredShading(def_OpaqueEntities, def_TransparentEntities);
	//	break;
	//}


	//m_FrameCount++;
}


void Scene::OnRenderUI()
{

}

void Scene::OnSceneDebug()
{
}


void Scene::OnDestroy()
{
	if (def_QuadMesh)
	{
		def_QuadMesh->Clear();
		def_QuadMesh = nullptr;
	}

	//Default Resources
	def_RenderableEntities.clear();
	def_OpaqueEntities.clear();
	def_TransparentEntities.clear();

	def_ForwardShader.Clear();
	def_DeferredShader.Clear();
	def_GBufferShader.Clear();
	def_SkyboxShader.Clear();

	def_GBuffer.Delete();

	def_Skybox.Destroy();

	def_CamMatUBO.Delete();
	m_LightDataUBO.Delete();
	m_EnviUBO.Delete();

	delete m_Camera;
	m_Camera = nullptr;

	window = nullptr;

	if (blank_tex)
		blank_tex->Clear();

	//for now hard reset 
	GLCall(glDisable(GL_DEPTH_TEST));
	GLCall(glDisable(GL_CULL_FACE));

}

void Scene::PreUpdateGPUUniformBuffers(Camera& cam)
{
	//------------------Camera Matrix Data UBO-----------------------------/
	unsigned int offset_ptr = 0;
	def_CamMatUBO.SetSubDataByID(&(cam.GetPosition()[0]), sizeof(glm::vec3), offset_ptr);
	offset_ptr += sizeof(glm::vec3);
	def_CamMatUBO.SetSubDataByID(cam.Ptr_Far(), sizeof(float), offset_ptr);
	offset_ptr += sizeof(float);
	def_CamMatUBO.SetSubDataByID(&(cam.CalculateProjMatrix(window->GetAspectRatio())[0][0]), sizeof(glm::mat4), offset_ptr);
	offset_ptr += sizeof(glm::mat4);
	def_CamMatUBO.SetSubDataByID(&(cam.CalViewMat()[0][0]), sizeof(glm::mat4), offset_ptr);
}

void Scene::IntermidateUpdateGPUUniformBuffers(DirectionalLight& d_light, std::vector<PointLight>& m_PtLights, const unsigned int& max_light)
{
	unsigned int offset_pointer = 0;
	offset_pointer = 0;
	d_light.UpdateUniformBufferData(m_LightDataUBO, offset_pointer);
	//for (int i = 0; i < MAX_POINT_LIGHT; i++)
		//ptLight[i].UpdateUniformBufferData(m_LightDataUBO, offset_pointer);

	//direction needs to be updated before point light 
	//has point light is dynamic, but the max light data are updated every frame
	//if only one light is available, we only update the available light 
	//and perform directional light before keeps the Light uniform buffer integrity 
	for (int i = 0; i < m_PtLights.size(); i++)
		if (i < max_light)
			m_PtLights[i].UpdateUniformBufferData(m_LightDataUBO, offset_pointer);


	//environment
	offset_pointer = 0;
	m_EnviUBO.SetSubDataByID(&b_EnableSkybox, sizeof(bool), offset_pointer);
	offset_pointer += sizeof(int);
	m_EnviUBO.SetSubDataByID(&m_SkyboxInfluencity, sizeof(float), offset_pointer);
	offset_pointer += sizeof(float);
	m_EnviUBO.SetSubDataByID(&m_SkyboxReflectivity, sizeof(float), offset_pointer);
	//m_SceneShader.SetUniform1i("u_SkyboxMap", 4);
}

void Scene::ForwardShading(std::vector<std::weak_ptr<RenderableMesh>> opaque_entities, std::vector<std::weak_ptr<RenderableMesh>> transparent_entities)
{
	//Start Rendering
	RenderCommand::Clear();

	def_ForwardShader.Bind();

	//move this to light uniform buffer
	def_ForwardShader.SetUniform1i("u_PtLightCount", 65);
	def_ForwardShader.SetUniform1i("u_SceneAsShadow", 0);

	//Draw Skybox
	if (b_EnableSkybox)
		def_Skybox.Draw(def_SkyboxShader, m_SceneRenderer);
	//Render Opaques entities
	//OpaquePass(def_ForwardShader, m_RenderableEntities);
	OpaquePass(def_ForwardShader, opaque_entities);

	//Transparent enitties
	TransparencyPass(def_ForwardShader, transparent_entities);
}

void Scene::DeferredShading(std::vector<std::weak_ptr<RenderableMesh>> opaque_entities, std::vector<std::weak_ptr<RenderableMesh>> transparent_entities)
{
	//Write scene opaque entities geometry attributes
	//into gbuffer (Base colour, Normal, Position)
	GBufferPass(def_GBufferShader, opaque_entities);


	//Perform Lighting calculations on Geomtric output images
	DeferredLightingPass(def_DeferredShader, def_GBuffer);


	//transparency pass
	if (transparent_entities.size() > 0)
	{
		//move this to camera uniform buffer
		//clear screen image texture depth created by deferred light padd
		RenderCommand::ClearDepthOnly();
		//get depth from gbuffer draw on default frame 
		//to test opaque depth with transparent objects
		def_GBuffer.BlitDepth();
		TransparencyPass(def_ForwardShader, transparent_entities);
	}
}

void Scene::OpaquePass(Shader& o_shader, std::vector<std::weak_ptr<RenderableMesh>> o_enitites)
{
	o_shader.Bind();
	for (const auto& e : o_enitites)
	{

		if (!e.expired())
		{
			auto& mesh = e.lock()->mesh.lock();
			auto& mat = e.lock()->material.lock();


			if (mesh)
			{
				if (mat)
					DefMaterialShaderBindHelper(*mat, o_shader);
				else//if no material, use first scene mat as default
					DefMaterialShaderBindHelper(*def_MeshMaterial, o_shader);


				o_shader.SetUniformMat4f("u_Model", *(e.lock()->transform));
				m_SceneRenderer.DrawMesh(mesh);
			}
		}
	}
}

void Scene::TransparencyPass(Shader& t_shader, std::vector<std::weak_ptr<RenderableMesh>> t_entities)
{
	t_shader.Bind();
	RenderCommand::EnableBlend();
	RenderCommand::EnableDepthTest();
	RenderCommand::BlendFactor(BlendFactors::SRC_ALPHA, BlendFactors::ONE_MINUS_SCR_A);
	for (const auto& e : t_entities)
	{
		if (!e.expired())
		{
			auto& mesh = e.lock()->mesh.lock();
			auto& mat = e.lock()->material.lock();


			if (mesh)
			{
				if (mat)
					DefMaterialShaderBindHelper(*mat, t_shader);
				else//if no material, use first scene mat as default
					DefMaterialShaderBindHelper(*def_MeshMaterial, t_shader);


				t_shader.SetUniformMat4f("u_Model", *(e.lock()->transform));
				m_SceneRenderer.DrawMesh(mesh);
			}

		}
	}
	RenderCommand::DisableBlend();
}

void Scene::GBufferPass(Shader& g_shader, std::vector<std::weak_ptr<RenderableMesh>> g_entities)
{
	//Write entities geometry attributes
	//into gbuffer (Base colour, Normal, Position)
	def_GBuffer.Bind();
	RenderCommand::Clear();
	RenderCommand::DisableBlend();
	//move this to camera uniform buffer
	g_shader.Bind();
	OpaquePass(g_shader, g_entities);
	RenderCommand::EnableBlend();
	def_GBuffer.UnBind();
}

void Scene::DeferredLightingPass(Shader& d_shader, MRTFramebuffer& g_render_targets)
{
	d_shader.Bind();
	d_shader.SetUniform1i("u_GBaseColourSpec", 0);
	g_render_targets.BindTextureIdx(0, 0);
	d_shader.SetUniform1i("u_GNormal", 1);
	g_render_targets.BindTextureIdx(1, 1);
	d_shader.SetUniform1i("u_GPosition", 2);
	g_render_targets.BindTextureIdx(2, 2);
	//not in use at the moment 
	//d_shader.SetUniform1i("u_GDepth", 3);
	//g_render_target.BindTextureIdx(3, 3);

	//move this to camera uniform buffer
	//move this to light uniform buffer
	d_shader.SetUniform1i("u_PtLightCount", 65);

	///m_Scre
	if (!def_QuadMesh)
		def_QuadMesh = CRRT::PrimitiveMeshFactory::Instance().CreateQuad();

	m_SceneRenderer.DrawMesh(def_QuadMesh);

	d_shader.UnBind();
}

void Scene::DefMaterialShaderBindHelper(Material& mat, Shader& shader)
{
	unsigned int tex_units = 0;
	shader.SetUniformVec4("u_Material.baseColour", mat.baseColour);
	if (mat.baseMap)
	{
		mat.baseMap->Activate(tex_units);
		shader.SetUniform1i("u_Material.baseMap", tex_units++);
	}
	if (mat.normalMap)
	{
		mat.normalMap->Activate(tex_units);
		shader.SetUniform1i("u_Material.normalMap", tex_units++);
	}
	if (mat.specularMap)
	{
		mat.specularMap->Activate(tex_units);
		shader.SetUniform1i("u_Material.specularMap", tex_units++);
	}
	
	shader.SetUniform1i("u_Material.useNormal", mat.useNormal && mat.normalMap);
	shader.SetUniform1i("u_Material.hasSpecularMap", (mat.specularMap != nullptr));
	shader.SetUniform1i("u_Material.shinness", mat.shinness);

}

void Scene::BuildRenderableMeshes(const std::shared_ptr<Entity>& entity)
{
	auto& mesh = entity->GetMesh();
	if (mesh)
	{
		auto& mat = entity->GetMaterial();
		
		RenderableMesh n_mesh
		{
			mesh,
			mat,
			entity->GetWorldTransformPtr(),
			entity->GetID()
		};

		std::shared_ptr<RenderableMesh> renderable_mesh = std::make_shared<RenderableMesh>(n_mesh);
		//renderable_mesh->mesh = mesh;
		//renderable_mesh->material = mat;
		//renderable_mesh->transform = entity->GetWorldTransform();

		def_RenderableEntities.emplace_back(renderable_mesh);
		
		if (mat)
		{
			switch (mat->renderMode)
			{
			case CRRT_Mat::RenderingMode::Transparent:
				def_TransparentEntities.emplace_back(renderable_mesh);
				break;
			case CRRT_Mat::RenderingMode::Opaque:
				def_OpaqueEntities.emplace_back(renderable_mesh);
				break;
			default:
				def_OpaqueEntities.emplace_back(renderable_mesh);
				break;
			}
		}
	}



	auto& children = entity->GetChildren();
	for (auto& c : children)
		BuildRenderableMeshes(c);
}

void Scene::BuildOpaqueTransparency(const std::vector<std::shared_ptr<RenderableMesh>> renderable_entities)
{
	def_TransparentEntities.clear();
	def_OpaqueEntities.clear();
	for (const auto& e : renderable_entities)
	{
		auto& mat = e->material.lock();

		if (mat)
		{
			switch (mat->renderMode)
			{
			case CRRT_Mat::RenderingMode::Transparent:
				def_TransparentEntities.emplace_back(e);
				break;
			case CRRT_Mat::RenderingMode::Opaque:
				def_OpaqueEntities.emplace_back(e);
				break;
			default:
				def_OpaqueEntities.emplace_back(e);
				break;
			}
		}
	}

	b_RebuildTransparency = false;
}

void Scene::SortByViewDistance(std::vector<std::weak_ptr<RenderableMesh>>& sorting_list)
{
	//probably move later
	for (const auto& e : sorting_list)
		e.lock()->UpdateViewSqrDist(m_Camera->GetPosition());

	std::sort(sorting_list.begin(), sorting_list.end(), RenderableMesh::CompareDistanceToView);

	b_ResortTransparency = false;
}


void Scene::UpdateShadersUniformBuffers()
{
	def_ForwardShader.Bind();
	def_ForwardShader.SetUniformBlockIdx("u_CameraMat", def_CamMatUBO.GetBindBlockIdx());
	def_ForwardShader.SetUniformBlockIdx("u_LightBuffer", m_LightDataUBO.GetBindBlockIdx());
	def_ForwardShader.SetUniformBlockIdx("u_EnvironmentBuffer", m_EnviUBO.GetBindBlockIdx());

	def_SkyboxShader.Bind();
	def_SkyboxShader.SetUniformBlockIdx("u_CameraMat", def_CamMatUBO.GetBindBlockIdx());

	//if Deferred Shadering / GBuffer
	def_GBufferShader.Bind();
	def_GBufferShader.SetUniformBlockIdx("u_CameraMat", def_CamMatUBO.GetBindBlockIdx());

	def_DeferredShader.Bind();
	def_DeferredShader.SetUniformBlockIdx("u_CameraMat", def_CamMatUBO.GetBindBlockIdx());
	def_DeferredShader.SetUniformBlockIdx("u_LightBuffer", m_LightDataUBO.GetBindBlockIdx());
}
