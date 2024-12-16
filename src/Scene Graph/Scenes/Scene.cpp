#pragma once
#include "Scene.h"
#include "Renderer/Renderer.h"

#include "Util/FilePaths.h"

#include "Scene Graph/Entity.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Material.h"
#include "Renderer/Shader.h"

#include "Renderer/Meshes/PrimitiveMeshFactory.h"

#include "Renderer/ObjectBuffer/ShadowMap.h"
#include "Renderer/RendererConfig.h"

#include "libs/imgui/imgui.h"
#include "Util/GPUStructure.h"

void Scene::SetWindow(Window* window)
{
	this->window = window;
}


void Scene::OnInit(Window* window)
{
	this->window = window;
	GLCall(glViewport(0, 0, window->GetWidth(), window->GetHeight()));

	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 1.0f/*0.5f*/);


	blank_tex = new Texture(FilePaths::Instance().GetPath("blank-image"));
}


void Scene::OnUpdate(float delta_time)
{
	if (b_EnableShadow)
	{
		def_DirShadowConfig.UpdateProjMat();
		def_DirShadowConfig.UpdateViewMatrix(def_DirLight.direction);
	}
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

void Scene::DefaultSetup()
{
	RenderCommand::EnableDepthTest();
	RenderCommand::EnableFaceCull();

	RenderCommand::CullBack();

	RenderCommand::DepthTestMode(DepthMode::LEEQUAL);
	//Other render behaivour / Data
	//Resolution setting 
	//Shadow config modification
	//etc 



	///////////////////////////////////////
	// Deferred Rendering Properties
	///////////////////////////////////////
	/*Shader m_DeferredShader;
	std::shared_ptr<Mesh> m_QuadMesh;
	Framebuffer m_ScreenFBO;*/
	SetQuadMesh(CRRT::PrimitiveMeshFactory::Instance().CreateQuad());
	ShaderFilePath shader_file_path
	{
		"Assets/Shaders/Deferred/DeferredShading.vert",
		"Assets/Shaders/Deferred/DeferredShading.frag"
	};
	def_DeferredShader.Create("deferred_shading", shader_file_path);



	def_ForwardShader.Create("model_forward_shading", CRRT_ASSETS_PATH"/Shaders/Forward/StandardVertex.glsl",
								CRRT_ASSETS_PATH"/Shaders/Forward/StandardFrag.glsl", "");
	def_ForwardShader.Bind();
	//texture unit Material::TextureCount + 1 >> 5
	def_ForwardShader.SetUniform1i("u_SkyboxMap", Material::MAX_MAP_COUNT + 1);


	
	def_GBufferShader.Create("gbuffer_shading", CRRT_ASSETS_PATH"/Shaders/Deferred/GBuffer.vert",
								CRRT_ASSETS_PATH"/Shaders/Deferred/GBuffer.frag", "");



	///////////////////////////////////////
	// GBUFFER
	///////////////////////////////////////
	std::vector<FBO_TextureImageConfig> fbo_img_config =
	{
		{FBO_Format::RGBA16F, GL_FLOAT},	//Basecolour-specular power buffer vec4
		{FBO_Format::RGB16F, GL_FLOAT, FBO_Format::RGB},	//Normal buffer
		{FBO_Format::RGB16F, GL_FLOAT, FBO_Format::RGB},	//Position buffer 
		//{FBO_Format::RGBA, GL_UNSIGNED_INT},	//Depth buffer with alpha as model mat shinness
		//use float for now and fix later
		{FBO_Format::RGB, GL_FLOAT, FBO_Format::RGB},	//Depth buffer with alpha as model mat shinness
	};
	def_GBuffer.Generate(window->GetWidth(), window->GetHeight(), fbo_img_config);





	//Lights 
	def_DirLight = DirectionalLight(glm::vec3(-0.65f, 3.45f, -0.9f),
		glm::vec3(1.0f), 0.086f, 0.966f, 0.5f);
	def_DirLight.enable = true;
	b_EnableShadow = true;

	//shadow map
	def_DirDepthMap.Generate(2048);
	def_DirShadowConfig.config.cam_far = 100.0f;
	def_DirShadowConfig.samplePos = glm::vec3(0.0f);
	def_DirShadowConfig.config.cam_near = 0.1f;
	def_DirShadowConfig.camOffset = 10.0f;
	def_DirShadowConfig.config.cam_size = 60.0f;


	def_ShadowDepthShader.Create("point_shadow_depth", CRRT_ASSETS_PATH"/Shaders/ShadowMapping/ShadowDepthVertex.glsl",
		CRRT_ASSETS_PATH"/Shaders/ShadowMapping/ShadowDepthFrag.glsl", CRRT_ASSETS_PATH"/Shaders/ShadowMapping/ShadowDepthGeometry.glsl");


	//point light shadow map 
	unsigned int count = 0;
	for (auto& pt : def_PtLights)
	{
		if (count > MAX_POINT_LIGHT_SHADOW)
			break;
		//using push_back instead of emplace_back 
		//to create a copy when storing in vector 
		def_PtDepthCubes.push_back(ShadowCube(1024));
		def_PtDepthCubes.back().Generate();
		count++;
	}


	//Entities
	b_EnableSkybox = false;
	m_SkyboxInfluencity = 0.205f;
	m_SkyboxReflectivity = 0.817f;
	///////////////////////////////////////////////////////////////////////
	// SKY BOX: Cube Texture Map
	///////////////////////////////////////////////////////////////////////
	std::vector<std::string> def_skybox_faces
	{
		CRRT_ASSETS_PATH"/Textures/Skyboxes/default_skybox/right.jpg",
		CRRT_ASSETS_PATH"/Textures/Skyboxes/default_skybox/left.jpg",
		CRRT_ASSETS_PATH"/Textures/Skyboxes/default_skybox/top.jpg",
		CRRT_ASSETS_PATH"/Textures/Skyboxes/default_skybox/bottom.jpg",
		CRRT_ASSETS_PATH"/Textures/Skyboxes/default_skybox/front.jpg",
		CRRT_ASSETS_PATH"/Textures/Skyboxes/default_skybox/back.jpg"
	};
	def_Skybox.Create(def_skybox_faces);
	def_Skybox.ActivateMap(4);
	//skybox shading
	def_SkyboxShader.Create("skybox_shader", CRRT_ASSETS_PATH"/Shaders/Utilities/Skybox/SkyboxVertex.glsl",
		CRRT_ASSETS_PATH"/Shaders/Utilities/Skybox/SkyboxFragment.glsl", "");


	////////////////////////////////////////
	// CREATE TEXTURES 
	////////////////////////////////////////
	auto plainMat = std::make_shared<Material>();
	plainMat->name = "Plane Material";
	plainMat->baseMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("plain"));

	auto floorMat = std::make_shared<Material>();
	floorMat->name = "Floor Mat";
	floorMat->baseMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("cobblestone-diff"));
	floorMat->normalMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("cobblestone-nor"));
	floorMat->parallaxMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("cobblestone-disp"));

	auto glassMat = std::make_shared<Material>();
	glassMat->name = "Glass Material";
	glassMat->baseMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("glass"));
	glassMat->renderMode = CRRT_Mat::RenderingMode::Transparent;
	glassMat->baseColour = glm::vec4(0.0f, 0.36f, 0.73f, 0.51f);

	auto glass2Mat = std::make_shared<Material>();
	glass2Mat->name = "Glass Material";
	glass2Mat->baseMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("glass"));

	def_MeshMaterial = plainMat;

	float entity_extra_scaleby = 1.0f;

	glm::mat4 temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(50.0f) * entity_extra_scaleby);

	//primitive construction
	std::shared_ptr<Mesh> m_QuadMesh = CRRT::PrimitiveMeshFactory::Instance().CreateQuad();
	std::shared_ptr<Mesh> cube_mesh = CRRT::PrimitiveMeshFactory::Instance().CreateCube();
	int id_idx = 0;


	m_entities.reserve(6);

	//floor 
	Entity floor_plane_entity = Entity(id_idx++, "floor-plane-entity", temp_trans, m_QuadMesh, floorMat);
	m_entities.emplace_back(std::make_shared<Entity>(floor_plane_entity));

	//move up 
	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.6f, 0.0f) * entity_extra_scaleby) *
				glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * entity_extra_scaleby);
	Entity cube_entity = Entity(id_idx++, "cube-entity", temp_trans, cube_mesh, plainMat);
	m_entities.emplace_back(std::make_shared<Entity>(cube_entity));

	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.5f, 0.0f) * entity_extra_scaleby) *
		glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * entity_extra_scaleby);
	Entity sphere_entity = Entity(id_idx++, "sphere-entity", temp_trans, CRRT::PrimitiveMeshFactory::Instance().CreateSphere(), plainMat);
	m_entities.emplace_back(std::make_shared<Entity>(sphere_entity));

	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 10.0f) * entity_extra_scaleby) *
		glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 1.0f) * entity_extra_scaleby);
	//glasses 
	Entity transparent_1 = Entity(id_idx++, "transparent_1_entity", temp_trans, cube_mesh, glassMat);
	m_entities.emplace_back(std::make_shared<Entity>(transparent_1));
	temp_trans = glm::translate(temp_trans, glm::vec3(0.0f, 0.0f, 10.0f));
	Entity transparent_2 = Entity(id_idx++, "transparent_2_entity", temp_trans, cube_mesh, glass2Mat);
	m_entities.emplace_back(std::make_shared<Entity>(transparent_2));
	temp_trans = glm::translate(temp_trans, glm::vec3(0.0f, 0.5f, -5.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.1f, 10.0f));
	Entity transparent_3 = Entity(id_idx++, "transparent_3_entity", temp_trans, cube_mesh, glassMat);
	m_entities.emplace_back(std::make_shared<Entity>(transparent_3));

	for (const auto& e : m_entities)
		BuildRenderableMeshes(e);


	////////////////////////////////////////
	// UNIFORM BUFFERs
	////////////////////////////////////////
	//------------------Camera Matrix Data UBO-----------------------------/
	long long int buf_size = sizeof(glm::vec3);// +sizeof(glm::mat3); //for view pos & mat3 padding
	buf_size += sizeof(float);// camera far
	buf_size += 2 * sizeof(glm::mat4);// +sizeof(glm::vec2);   //to store view, projection
	def_CamMatUBO.Generate(buf_size);
	def_CamMatUBO.BindBufferRndIdx(0, buf_size, 0);

	//------------------Light Data UBO-----------------------------/
	//struct
	//DirectionalLight dirLight;
	//PointLight pointLights[MAX_POINT_LIGHTS];
	long long int light_buffer_size = 0;
	light_buffer_size += DirectionalLight::GetGPUSize();
	//Point Light
	light_buffer_size += PointLight::GetGPUSize() * MAX_POINT_LIGHT;
	m_LightDataUBO.Generate(light_buffer_size);
	m_LightDataUBO.BindBufferRndIdx(1, light_buffer_size, 0);


	UpdateShadersUniformBuffers();


	//------------------Enviroment Data UBO-----------------------------/
	long long int envi_buffer_size = CRRT::EnvironmentData::GetGPUSize();
	m_EnviUBO.Generate(envi_buffer_size);
	m_EnviUBO.BindBufferRndIdx(2, envi_buffer_size, 0);

	//Assign UBO, if necessary 
}

void Scene::DefaultSceneRendering()
{

	//Pre Rendering
	//BeginRenderScene();
	PreUpdateGPUUniformBuffers(*m_Camera);
	//naive flat out self - children - children into list/collection, easy mutilple interation

	if (b_RebuildTransparency)
	{
		BuildOpaqueTransparency(def_RenderableEntities);
		SortByViewDistance(def_TransparentEntities);
	}

	if (b_ResortTransparency)
		SortByViewDistance(def_TransparentEntities);

	SortByViewDistance(def_TransparentEntities);


	IntermidateUpdateGPUUniformBuffers();


	if (b_EnableShadow)
		DefaultShadowPass(def_ShadowDepthShader, def_RenderableEntities);


	switch (m_RenderingPath)
	{
	case Forward:
		ForwardShading(def_OpaqueEntities, def_TransparentEntities);
		break;
	case Deferred:
		DeferredShading(def_OpaqueEntities, def_TransparentEntities);
		break;
	}


	m_FrameCount++;


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

void Scene::IntermidateUpdateGPUUniformBuffers()
{
	unsigned int offset_pointer = 0;
	offset_pointer = 0;
	def_DirLight.UpdateUniformBufferData(m_LightDataUBO, offset_pointer);
	//for (int i = 0; i < MAX_POINT_LIGHT; i++)
		//ptLight[i].UpdateUniformBufferData(m_LightDataUBO, offset_pointer);

	//direction needs to be updated before point light 
	//has point light is dynamic, but the max light data are updated every frame
	//if only one light is available, we only update the available light 
	//and perform directional light before keeps the Light uniform buffer integrity 
	for (int i = 0; i < def_PtLights.size(); i++)
		if (i < MAX_POINT_LIGHT)
			def_PtLights[i].UpdateUniformBufferData(m_LightDataUBO, offset_pointer);


	//environment
	offset_pointer = 0;
	m_EnviUBO.SetSubDataByID(&b_EnableSkybox, sizeof(bool), offset_pointer);
	offset_pointer += sizeof(int);
	m_EnviUBO.SetSubDataByID(&m_SkyboxInfluencity, sizeof(float), offset_pointer);
	offset_pointer += sizeof(float);
	m_EnviUBO.SetSubDataByID(&m_SkyboxReflectivity, sizeof(float), offset_pointer);
	//m_SceneShader.SetUniform1i("u_SkyboxMap", 4);MAX_
}

void Scene::ForwardShading(std::vector<std::weak_ptr<RenderableMesh>>& opaque_entities, std::vector<std::weak_ptr<RenderableMesh>>& transparent_entities)
{
	//Start Rendering
	RenderCommand::Clear();

	def_ForwardShader.Bind();

	//////////////////////////////////////////////
	// Only Forward Rendering Path as Shadow
	//////////////////////////////////////////////
	//m_ForwardShader.SetUniform1i("u_EnableSceneShadow", m_EnableShadows);
	if (b_EnableShadow && b_FrameHasShadow)
	{
		def_ForwardShader.SetUniform1i("u_SceneAsShadow", 1);
		def_ForwardShader.SetUniformMat4f("u_DirLightSpaceMatrix", def_DirShadowConfig.GetLightSpaceMatrix());
		//tex unit 0 >> texture (base map)
		//tex unit 1 >> potenially normal map
		//tex unit 2 >> potenially parallax map
		//tex unit 3 >> potenially specular map
		//tex unit 4 >> shadow map (dir Light)
		//tex unit 5 >> skybox cube map
		//tex unit 6 >> shadow cube (pt Light)
		def_DirDepthMap.Read(Material::MAX_MAP_COUNT);
		///////////////////////////////////////////
		// Test with gbuffer base colour 
		///////////////////////////////////////////
		def_ForwardShader.SetUniform1i("u_DirShadowMap", Material::MAX_MAP_COUNT);

		//point light shadow starts at  Material::TextureCount + 2 >> 6
		for (int i = 0; i < def_PtLights.size(); i++)
		{
			if (i > MAX_POINT_LIGHT_SHADOW)
				break;

			def_PtDepthCubes[i].Read(Material::MAX_MAP_COUNT + 2 + i);
			def_ForwardShader.SetUniform1i(("u_PointShadowCubes[" + std::to_string(i) + "]").c_str(), Material::MAX_MAP_COUNT + 2 + i);
		}
	}
	else
		def_ForwardShader.SetUniform1i("u_SceneAsShadow", 0);


	//move this to light uniform buffer
	def_ForwardShader.SetUniform1i("u_PtLightCount", def_PtLights.size());

	//Draw Skybox
	if (b_EnableSkybox)
		def_Skybox.Draw(def_SkyboxShader, m_SceneRenderer);
	//Render Opaques entities
	//OpaquePass(def_ForwardShader, m_RenderableEntities);
	OpaquePass(def_ForwardShader, opaque_entities);

	//Transparent enitties
	TransparencyPass(def_ForwardShader, transparent_entities);
}

void Scene::DeferredShading(std::vector<std::weak_ptr<RenderableMesh>>& opaque_entities, std::vector<std::weak_ptr<RenderableMesh>>& transparent_entities)
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

void Scene::OpaquePass(Shader& o_shader, std::vector<std::weak_ptr<RenderableMesh>>& o_enitites)
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

void Scene::TransparencyPass(Shader& t_shader, std::vector<std::weak_ptr<RenderableMesh>>& t_entities)
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

void Scene::GBufferPass(Shader& g_shader, std::vector<std::weak_ptr<RenderableMesh>>& g_entities)
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
	d_shader.SetUniform1i("u_PtLightCount", def_PtLights.size());

	///m_Scre
	if (!def_QuadMesh)
		def_QuadMesh = CRRT::PrimitiveMeshFactory::Instance().CreateQuad();

	m_SceneRenderer.DrawMesh(def_QuadMesh);

	d_shader.UnBind();
}

void Scene::DefaultShadowPass(Shader& depth_shader, std::vector<std::shared_ptr<RenderableMesh>>& entities)
{

	RenderCommand::CullFront();

	////////////////////////
	// Directional Shadow
	////////////////////////
	depth_shader.Bind();
	def_DirDepthMap.Write();
	RenderCommand::ClearDepthOnly();
	depth_shader.SetUniform1i("u_IsOmnidir", 0);
	//ditrectional ligth projection and view
	//glm::mat4 proj = glm::ortho(-config.cam_size, config.cam_size,
	//				  -config.cam_size, config.cam_size, 
	//				  config.cam_near, config.cam_far);
	////                                    //offset from focus
	//glm::mat4 view = glm::lookAt(def_DirLight.direction * 64.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 light_mat = proj * view;
	depth_shader.SetUniformMat4f("u_LightSpaceMat", def_DirShadowConfig.GetLightSpaceMatrix());

	for (const auto& m : entities)
	{
		depth_shader.SetUniformMat4f("u_Model", *m->transform);
		m_SceneRenderer.DrawMesh(m->mesh.lock());
	}
	def_DirDepthMap.UnBind();


	////////////////////////////
	// Point Light Shadow
	////////////////////////////

	bool do_pt_shadow = true;
	if (def_PtLights.size() > 0 && do_pt_shadow)
	{
		std::vector<glm::mat4> shadow_mats = PointShadowCalculation::PointLightSpaceMatrix(def_PtLights[0].position);
		depth_shader.SetUniform1i("u_IsOmnidir", 1);
		depth_shader.SetUniform1f("u_FarPlane", def_PtLights[0].shadow_far);

		for (unsigned int i = 1; i < def_PtLights.size(); i++)
		{
			if (i > MAX_POINT_LIGHT_SHADOW)
				break;

			depth_shader.SetUniformVec3("u_LightPos", def_PtLights[i].position);
			shadow_mats = PointShadowCalculation::PointLightSpaceMatrix(def_PtLights[i].position);
			for (int f = 0; f < 6; ++f)
			{
				depth_shader.SetUniformMat4f(("u_ShadowMatrices[" + std::to_string(f) + "]").c_str(), shadow_mats[f]);
			}

			def_PtDepthCubes[i].Write();//ready to write in the depth cube framebuffer for light "i"
			RenderCommand::ClearDepthOnly();//clear the depth buffer 

			//draw renderable meshes 
			for (const auto& m : entities)
			{
				depth_shader.SetUniformMat4f("u_Model", *m->transform);
				m_SceneRenderer.DrawMesh(m->mesh.lock());
			}
			//unbind current point light shadow cube
			def_PtDepthCubes[i].UnBind();
		}
	}


	//done with shadow calcultaion 
	depth_shader.UnBind();
	RenderCommand::CullBack();
	RenderCommand::Viewport(0, 0, window->GetWidth(), window->GetHeight());
	b_FrameHasShadow = true;
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

void Scene::BuildOpaqueTransparency(const std::vector<std::shared_ptr<RenderableMesh>>& renderable_entities)
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

void Scene::RefreshFrame()
{	
	b_FrameHasShadow = false;
}

void Scene::ResizeBuffers(unsigned int width, unsigned int height)
{
	m_PrevViewWidth = width;
	m_PrevViewHeight = height;
	def_GBuffer.ResizeBuffer(width, height);
}

void Scene::DebugDisplayDirectionalLightUIPanel(unsigned int scale)
{
	if (ImGui::TreeNode("Directional Shadow Depth"))
	{
		scale = (scale == 0) ? 1 : scale;
		ImVec2 img_size(500.0f * scale, 500.0f * scale);
		//img_size.y *= (def_DirDepthMap.GetSize().y / def_DirDepthMap.GetSize()); //invert
		ImGui::SeparatorText("Direction light shadow depth view");
		ImGui::Image((ImTextureID)(intptr_t)def_DirDepthMap.GetColourAttactment(), img_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::TreePop();
	}
}

void Scene::SceneDefaultUI()
{
	DefaultMainUI();
	DefaultGBufferDisplayUI();
}

void Scene::DefaultMainUI()
{
	ImGui::Begin("Default Scene Panel");
	WindowInfoUIPanel();
	CameraUIPanel();
	ScenePropUIPanel();
	Ext_MainUI_LightTree();
	ImGui::End();
}

void Scene::DefaultGBufferDisplayUI()
{
	if (ImGui::Begin("Scene default buffers"))
	{
		static int scale = 1;
		ImGui::SliderInt("image scale", &scale, 1, 5);

		DebugDisplayDirectionalLightUIPanel(scale);


		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::TreeNode("GBuffer"))
		{
			ImVec2 img_size(500.0f * scale, 500.0f * scale);
			img_size.y *= (def_GBuffer.GetSize().y / def_GBuffer.GetSize().x); //invert
			ImGui::Text("Colour Attachment Count: %d", def_GBuffer.GetColourAttachmentCount());
			//render image base on how many avaliable render tragets
			for (unsigned int i = 0; i < def_GBuffer.GetColourAttachmentCount(); i++)
			{
				ImGui::PushID(&i);//use this id 
				ImGui::Separator();
				ImGui::Image((ImTextureID)(intptr_t)def_GBuffer.GetColourAttachment(i), img_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}

void Scene::WindowInfoUIPanel()
{
	ImGui::SeparatorText("Window info");
	ImGui::Text("Win Title: %s", window->GetInitProp()->title);
	ImGui::Text("Window Width: %d", window->GetWidth());
	ImGui::Text("Window Height: %d", window->GetHeight());

	ImGui::Text("Init Width: %d", window->GetInitProp()->width);
	ImGui::Text("Init Height: %d", window->GetInitProp()->height);
	
}

void Scene::CameraUIPanel()
{
	ImGui::SeparatorText("Camera info");
	ImGui::Text("Position x: %f, y: %f, z: %f",
		m_Camera->GetPosition().x,
		m_Camera->GetPosition().y,
		m_Camera->GetPosition().z);

	ImGui::Text("Pitch: %f", m_Camera->Ptr_Pitch());
	ImGui::Text("Yaw: %f", m_Camera->Ptr_Yaw());


	if (ImGui::TreeNode("Camera Properties"))
	{
		ImGui::SliderFloat("Move Speed", m_Camera->Ptr_MoveSpeed(), 5.0f, 250.0f);
		ImGui::SliderFloat("Rot Speed", m_Camera->Ptr_RotSpeed(), 0.0f, 2.0f);

		float window_width = (float)window->GetWidth();
		float window_height = (float)window->GetHeight();
		static glm::mat4 test_proj;

		bool update_camera_proj = false;

		update_camera_proj = ImGui::SliderFloat("FOV", m_Camera->Ptr_FOV(), 0.0f, 179.0f, "%.1f");
		update_camera_proj |= ImGui::DragFloat("Near", m_Camera->Ptr_Near(), 0.1f, 0.1f, 50.0f, "%.1f");
		update_camera_proj |= ImGui::DragFloat("Far", m_Camera->Ptr_Far(), 0.1f, 0.0f, 500.0f, "%.1f");

		if (update_camera_proj)
		{
			glm::mat4 new_proj = m_Camera->CalculateProjMatrix(window->GetAspectRatio());
			//m_MainRenderer2.UpdateShaderViewProjection(new_proj);
		}


		ImGui::TreePop();
	}
}

void Scene::ScenePropUIPanel()
{
	ImGui::Spacing();
	ImGui::SeparatorText("Scene Properties");
	ImGui::ColorEdit3("clear Screen Colour", &m_ClearScreenColour[0]);
	Ext_MainUI_SceneTree();
	static int curr_value = 0;
	const char* render_paths[] = { "Forward", "Deferred" };
	if (ImGui::Combo("Rendering Path", &curr_value, render_paths, 2))
		m_RenderingPath = (RenderingPath)curr_value;

	ImGui::Spacing();
	ImGui::SeparatorText("Scene Entities");
	ImGui::Text("Entities counts %d", def_RenderableEntities.size());
	ImGui::Text("Opaque entities %d", def_OpaqueEntities.size());
	ImGui::Text("Transparent entities %d", def_TransparentEntities.size());

	ImGui::Text("Transparent Order: ");
	for (const auto& e : def_TransparentEntities)
		if (!e.expired())
		{
			ImGui::SameLine();
			ImGui::Text("%d", e.lock()->objectID);
		}
}


void Scene::Ext_MainUI_LightTree()
{
	if (ImGui::TreeNode("Lights"))
	{

		//////////////////////////////////////
		// GENERAL LIGHT PROPS
		//////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Light Global Properties");
		ImGui::Checkbox("Enable Scene Shadow", &b_EnableShadow);

		//////////////////////////////////////
		// ENVIRONMENT SKYBOX
		//////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Environment");
		if (ImGui::TreeNode("Skybox"))
		{
			ImGui::Checkbox("Use Skybox", &b_EnableSkybox);
			ImGui::SliderFloat("Skybox influencity", &m_SkyboxInfluencity, 0.0f, 1.0f);
			ImGui::SliderFloat("Skybox reflectivity", &m_SkyboxReflectivity, 0.0f, 1.0f);

			ImGui::TreePop();
		}

		//////////////////////////////////////
		// Directional Light
		//////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Directional Light");
		if (ImGui::TreeNode("Directional Light"))
		{
			ImGui::Checkbox("Enable Directional", &def_DirLight.enable);
			//ImGui::SameLine();
			//ImGui::Checkbox("Cast Shadow", &def_DirLight.castShadow);
			ImGui::DragFloat3("Light Direction", &def_DirLight.direction[0], 0.05f, -5.0f, 5.0f);
			//ImGui::DragFloat3("Light Direction", &def_DirLight.direction[0], 0.1f, -1.0f, 1.0f);
			ImGui::ColorEdit3("Dir Light colour", &def_DirLight.colour[0]);
			ImGui::SliderFloat("Light ambinentIntensity", &def_DirLight.ambientIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Light diffuseIntensity", &def_DirLight.diffuseIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Light specIntensity", &def_DirLight.specularIntensity, 0.0f, 1.0f);
			ImGui::Checkbox("Light Can Shadow", &def_DirLight.castShadow);
			ImGui::TreePop();
		}


		//////////////////////////////////////////
		// Point Lights
		//////////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Point Lights");
		if (ImGui::TreeNode("Points Lights"))
		{
			for (int i = 0; i < def_PtLights.size(); i++)
			{
				if (i > MAX_POINT_LIGHT)
					break;

				std::string label = "point light: " + std::to_string(i);
				ImGui::SeparatorText(label.c_str());
				ImGui::Checkbox((label + " Enable light").c_str(), &def_PtLights[i].enable);

				ImGui::DragFloat3((label + " position").c_str(), &def_PtLights[i].position[0], 0.1f);

				ImGui::ColorEdit3((label + " colour").c_str(), &def_PtLights[i].colour[0]);
				ImGui::SliderFloat((label + " ambinentIntensity").c_str(), &def_PtLights[i].ambientIntensity, 0.0f, 1.0f);
				ImGui::SliderFloat((label + " diffuseIntensity").c_str(), &def_PtLights[i].diffuseIntensity, 0.0f, 1.0f);
				ImGui::SliderFloat((label + " specIntensity").c_str(), &def_PtLights[i].specularIntensity, 0.0f, 1.0f);
				ImGui::SliderFloat((label + " constant attenuation").c_str(), &def_PtLights[i].attenuation[0], 0.0f, 1.0f);
				ImGui::SliderFloat((label + " linear attenuation").c_str(), &def_PtLights[i].attenuation[1], 0.0f, 1.0f);
				ImGui::SliderFloat((label + " quadratic attenuation").c_str(), &def_PtLights[i].attenuation[2], 0.0f, 1.0f);
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void Scene::Ext_MainUI_SceneTree()
{
	ImGui::Spacing();
	if (ImGui::TreeNode("Debug Scene"))
	{
		ImGui::Checkbox("Allow Scene Debug Gizmos", &b_DebugScene);
		if (b_DebugScene)
		{
			ImGui::Checkbox("Debug Dir Shadow Para", &def_DirShadowConfig.debugPara);
			if (def_DirLight.castShadow)
			{
				if (ImGui::TreeNode("Shadow Camera Info"))
				{
					auto& shadow = def_DirShadowConfig;
					ImGui::SliderFloat("Camera Near", &shadow.config.cam_near, 0.0f, shadow.config.cam_far - 0.5f);
					ImGui::SliderFloat("Camera Far", &shadow.config.cam_far, shadow.config.cam_near + 0.5f, 1000.0f);
					ImGui::SliderFloat("Camera Size", &shadow.config.cam_size, 0.0f, 200.0f);
					ImGui::DragFloat3("Sample Pos", &def_DirShadowConfig.samplePos[0], 0.1f);
					ImGui::SliderFloat("Light Proj Offset", &def_DirShadowConfig.camOffset, 0.0f, 100.0f);
					ImGui::Checkbox("Debug Dir Shadow Para", &shadow.debugPara);

					ImGui::TreePop();
				}
			}
		}
		else
			ImGui::Text("Check Allow Scene Debug Gizmos");
		ImGui::TreePop();
	}
}
