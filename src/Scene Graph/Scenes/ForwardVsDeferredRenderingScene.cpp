#include "ForwardVsDeferredRenderingScene.h"

#include "Renderer/RenderCommand.h"

#include "Renderer/Material.h"

#include "Util/FilePaths.h"
#include "Util/GPUStructure.h"
#include "Renderer/Meshes/PrimitiveMeshFactory.h"

#include "libs/imgui/imgui.h"
#include "Util/MathsHelpers.h"
#include <glm/gtx/quaternion.hpp>


#include "Renderer/DebugGizmos.h"

//for serialisering & deseralising scene data
#include "Util/SceneSerialiser.h"
#include <iostream>
#include <fstream>


#define MAT_TEXTURE_COUNT 4

void ForwardVsDeferredRenderingScene::SetWindow(Window* window)
{
	this->window = window;
}

void ForwardVsDeferredRenderingScene::OnInit(Window* window)
{
	Scene::OnInit(window);
	window->UpdateProgramTitle("Forward vs Deferred Rendering Scene");

	//Need to move out 
	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 1.0f/*0.5f*/);


	InitRenderer();
	SceneData scene_data = LoadSceneFromFile();
	//Load entities
	SetRenderingConfiguration(scene_data.m_RenderingConfig);
	CreateLightsAndShadowDatas(scene_data.m_Light, scene_data.m_Shadow);
	CreateEntities(scene_data);
	//all generate and scene data should be ready before creating GPU specific datas
	CreateGPUDatas();
}

void ForwardVsDeferredRenderingScene::OnUpdate(float delta_time)
{
	//definately need to move this out
	if (m_EnableShadows)
	{
		dirLightObject.dirLightShadow.UpdateProjMat();
		dirLightObject.dirLightShadow.UpdateViewMatrix(dirLightObject.sampleWorldPos,
			dirLightObject.dirlight.direction,
			dirLightObject.cam_offset);
	}
	//point shadow far update 
	float shfar = m_PtShadowConfig.cam_far;
	for (int i = 0; i < m_PtLightCount; i++)
		m_PtLights[i].shadow_far = shfar;

	glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 desired_move_dir;
	glm::vec3 pt_orbit_dir_xz; //ignore direction on y 


	bool hack = false; 
	if (hack)
		m_OrbitSpeed = 0.0f;

	for (int i = 0; i < m_PtLightCount; i++)
	{
		pt_orbit_dir_xz = m_PtOrbitOrigin - m_PtLights[i].position;
		pt_orbit_dir_xz.y = 0.0f; //no difference on y axis

		//check if not too far
		float init_dist = glm::length(pt_orbit_dir_xz);
		pt_orbit_dir_xz = glm::normalize(pt_orbit_dir_xz);

		//12.442357, 15.3, 15.658475
		//adjust / force to required 
		if (init_dist > m_DesiredDistance)
			m_PtLights[i].position += pt_orbit_dir_xz * (init_dist - m_DesiredDistance);


		desired_move_dir = glm::cross(world_up, pt_orbit_dir_xz);
		m_PtLights[i].position += desired_move_dir * m_OrbitSpeed * delta_time * 5.0f; // based on entity_extra_scaleby
	}


	OnRender();

	if (m_PrevViewWidth != window->GetWidth() || m_PrevViewHeight != window->GetHeight())
		ResizeBuffers(window->GetWidth(), window->GetHeight());

	ResetSceneFrame();


	//m_ShaderHotReload.Update();


	//m_ForwardShader.Bind();
	//m_ForwardShader.SetUniformBlockIdx("u_CameraMat", 0);
	//m_ForwardShader.SetUniformBlockIdx("u_LightBuffer", 1);
	//m_ForwardShader.SetUniformBlockIdx("u_EnvironmentBuffer", 2);
}

void ForwardVsDeferredRenderingScene::OnRender()
{
	//Pre Rendering
	BeginRenderScene();
	PreUpdateGPUUniformBuffers(*m_Camera);
	//naive flat out self - children - children into list/collection, easy mutilple interation

	if (m_FrameCount < 1)
	{
		for (const auto& e : m_SceneEntities)
			BuildRenderableMeshes(e);

		////Build renderable meshes flats out the mesh and sort by grp {Opaque, Transparency}
		////so if transparent lets sort by view. 
		if (def_TransparentEntities.size() > 1)
			SortByViewDistance(def_TransparentEntities);
	}



	if (b_RebuildTransparency)
	{
		BuildOpaqueTransparency(def_RenderableEntities);
		SortByViewDistance(def_TransparentEntities);
	}

	if (b_ResortTransparency)
		SortByViewDistance(def_TransparentEntities);
		
	SortByViewDistance(def_TransparentEntities);

	IntermidateUpdateGPUUniformBuffers(dirLightObject.dirlight, m_PtLights, MAX_POINT_LIGHT);

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

	if(m_DebugScene)
		OnSceneDebug();

	//return;
}

void ForwardVsDeferredRenderingScene::OnRenderUI()
{
	MainUI();
	EnititiesUI();
	MaterialsUI();
	GBufferDisplayUI();
	//ScreenFBODisplayUI();
}

void ForwardVsDeferredRenderingScene::OnDestroy()
{
	if (m_PtLightCount > 0)
		SerialiseScene();


	Scene::OnDestroy();
}

void ForwardVsDeferredRenderingScene::InitRenderer()
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


}

void ForwardVsDeferredRenderingScene::CreateEntities(const SceneData&  scene_data)
{
	//////////////////////////////////////
	// GENERATE SHADERS
	//////////////////////////////////////
	//model shader


	auto& envir_data = scene_data.m_SceneEnvi;
	b_EnableSkybox = envir_data.m_EnableSkybox;
	m_SkyboxInfluencity = envir_data.m_SkyboxInfluencity;
	m_SkyboxReflectivity = envir_data.m_SkyboxReflectivity;
	///////////////////////////////////////////////////////////////////////
	// SKY BOX: Cube Texture Map
	///////////////////////////////////////////////////////////////////////
	std::vector<std::string> def_skybox_faces
	{
		envir_data.m_SkyboxTex.tex_rt,
		envir_data.m_SkyboxTex.tex_lt,
		envir_data.m_SkyboxTex.tex_tp,
		envir_data.m_SkyboxTex.tex_bm,
		envir_data.m_SkyboxTex.tex_ft,
		envir_data.m_SkyboxTex.tex_bk,
	};
	def_Skybox.Create(def_skybox_faces);
	def_Skybox.ActivateMap(4);
	//skybox shading
	def_SkyboxShader.Create(envir_data.m_SkyboxShader.m_Name, envir_data.m_SkyboxShader.m_Vertex, 
						 envir_data.m_SkyboxShader.m_Fragment, envir_data.m_SkyboxShader.m_Geometry);

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
	////////////////////////////////////////
	// CREATE ENTITIES 
	////////////////////////////////////////

	//TEST TEST TEST TEST 
	//kind of like world scale
	float entity_extra_scaleby = 5.0f;

	glm::mat4 temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
						   glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
						   glm::scale(glm::mat4(1.0f), glm::vec3(50.0f) * entity_extra_scaleby); 

	//primitive construction
	std::shared_ptr<Mesh> m_QuadMesh = CRRT::PrimitiveMeshFactory::Instance().CreateQuad();
	std::shared_ptr<Mesh> cube_mesh = CRRT::PrimitiveMeshFactory::Instance().CreateCube();
	int id_idx = 0;

	//floor 
	Entity floor_plane_entity = Entity(id_idx++, "floor-plane-entity", temp_trans, m_QuadMesh, floorMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(floor_plane_entity));

	//move up 
	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.6f, 0.0f) * entity_extra_scaleby) * 
				 glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * entity_extra_scaleby);
	Entity cube_entity = Entity(id_idx++, "cube-entity", temp_trans, cube_mesh, plainMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(cube_entity));

	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.5f, 0.0f) * entity_extra_scaleby) *
				 glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * entity_extra_scaleby);
	Entity sphere_entity = Entity(id_idx++, "sphere-entity", temp_trans, CRRT::PrimitiveMeshFactory::Instance().CreateSphere(), plainMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(sphere_entity));

	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 10.0f) * entity_extra_scaleby) *
				glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 1.0f) * entity_extra_scaleby);
	//glasses 
	Entity transparent_1 = Entity(id_idx++, "transparent_1_entity", temp_trans, cube_mesh, glassMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(transparent_1));
	temp_trans = glm::translate(temp_trans, glm::vec3(0.0f, 0.0f, 10.0f));
	Entity transparent_2 = Entity(id_idx++, "transparent_2_entity", temp_trans, cube_mesh, glass2Mat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(transparent_2));
	temp_trans = glm::translate(temp_trans, glm::vec3(0.0f, 0.5f, -5.0f)) *
				 glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.1f, 10.0f));
	Entity transparent_3 = Entity(id_idx++, "transparent_3_entity", temp_trans, cube_mesh, glassMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(transparent_3));


	//testing sponza
	temp_trans = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
				 glm::scale(glm::mat4(1.0f), glm::vec3(0.05f) * entity_extra_scaleby);
	std::shared_ptr<Entity> sponza_model = m_NewModelLoader.LoadAsEntity(FilePaths::Instance().GetPath("sponza"), true);
	sponza_model->SetLocalTransform(temp_trans);
	m_SceneEntities.emplace_back(sponza_model);




}
void ForwardVsDeferredRenderingScene::CreateLightsAndShadowDatas(const SceneLightData& light_data, const SceneShadowData& shadow_data)
{
	m_PtLightCount = light_data.m_PtLightCount;

	if (m_PtLightCount > 0)
		m_PtLights = light_data.ptLights;

	//shadow map
	dirDepthMap.Generate(shadow_data.m_DirDepthResQuality);
	dirLightObject.dirLightShadow.config.cam_far = shadow_data.m_DirZFar;
	dirLightObject.sampleWorldPos = shadow_data.m_DirSamplePos;
	dirLightObject.dirLightShadow.config.cam_far = shadow_data.m_DirZFar;
	dirLightObject.cam_offset = shadow_data.m_DirSampleOffset;
	dirLightObject.dirLightShadow.config.cam_size = shadow_data.m_DirOrthoSize;


	m_ShadowDepthShader.Create("point_shadow_depth", shadow_data.m_depthVerShader,
		shadow_data.m_depthFragShader, shadow_data.m_depthGeoShader);


	//point light shadow map 
	unsigned int count = 0;
	for (auto& pt : m_PtLights)
	{
		if (count > MAX_POINT_LIGHT_SHADOW)
			break;
		//using push_back instead of emplace_back 
		//to create a copy when storing in vector 
		m_PtDepthMapCubes.push_back(ShadowCube(shadow_data.m_PtDepthResQuality));
		m_PtDepthMapCubes.back().Generate();
		count++;
	}



}

void ForwardVsDeferredRenderingScene::SetRenderingConfiguration(const SceneRenderingConfig& scene_render_config)
{
	
	ShaderFilePath shader_file_path
	{
		scene_render_config.m_ForwardShader.m_Vertex,
		scene_render_config.m_ForwardShader.m_Fragment,
		scene_render_config.m_ForwardShader.m_Geometry,
	};
	def_ForwardShader.Create(scene_render_config.m_ForwardShader.m_Name, shader_file_path);
	def_ForwardShader.Bind();
	//texture unit Material::TextureCount + 1 >> 5
	def_ForwardShader.SetUniform1i("u_SkyboxMap", MAT_TEXTURE_COUNT + 1);


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

	auto& shader_data = scene_render_config.m_GBufferShader;
	def_GBufferShader.Create(shader_data.m_Name, shader_data.m_Vertex,
		shader_data.m_Fragment, shader_data.m_Geometry);

	//Track forward shader for hot reloading
	//m_ShaderHotReload.TrackShader(&m_ForwardShader);

}


void ForwardVsDeferredRenderingScene::CreateGPUDatas()
{
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



	//------------------Enviroment Data UBO-----------------------------/
	long long int envi_buffer_size = CRRT::EnvironmentData::GetGPUSize();
	m_EnviUBO.Generate(envi_buffer_size);
	m_EnviUBO.BindBufferRndIdx(2, envi_buffer_size, 0);

	//Assign UBO, if necessary 
	UpdateShadersUniformBuffers();
}

SceneData ForwardVsDeferredRenderingScene::LoadSceneFromFile()
{
	//--------------------Light--------------------------------/
	//Dir light
	auto& dl = dirLightObject.dirlight;

	SceneData scene_data{};
	if (!SceneSerialiser::Instance().LoadScene("Assets/Scene/experiment.crrtscene", scene_data))
		printf("[SCENE FILE LOADING]: Failed to retrive all data from file!!!!!!!\n");


	m_EnableShadows = scene_data.m_HasShadow;

	//Overwrite Scene camera for now
	//its better to change the address because other
	//pointer could be affected 
	auto cam_load_data = scene_data.m_Camera;
	m_Camera = new Camera(cam_load_data.m_Position, glm::vec3(0.0f, 1.0f, 0.0f),
		cam_load_data.m_Yaw, cam_load_data.m_Pitch,
		cam_load_data.m_MoveSpeed, cam_load_data.m_RotSpeed);


	//force set Fov, near and far
	*m_Camera->Ptr_FOV() = cam_load_data.m_Fov;
	*m_Camera->Ptr_Near() = cam_load_data.m_Near;
	*m_Camera->Ptr_Far() = cam_load_data.m_Far;

	return scene_data;
}

void ForwardVsDeferredRenderingScene::SerialiseScene()
{
	//experiment with loading data
	SceneData scene_info_data
	{
		"Forward Vs Deferred Scene",
		true,	//m_HasLight;
		m_EnableShadows, //true,	//m_HasShadow = true;
		true,	//m_HasEnivornmentMap = true;
		//camera 
		{
			m_Camera->GetPosition(),
			m_Camera->Ptr_Yaw(),
			m_Camera->Ptr_Pitch(),
			*m_Camera->Ptr_FOV(),
			*m_Camera->Ptr_Near(),
			*m_Camera->Ptr_Far(),
			*m_Camera->Ptr_MoveSpeed(),
			*m_Camera->Ptr_RotSpeed(),
		},
		//Rendering config
		{
			//forward rendering shader
			{
				def_ForwardShader.GetName(),
				//REALLY BAD,
				(def_ForwardShader.GetShaderFilePath(GL_VERTEX_SHADER)),
				(def_ForwardShader.GetShaderFilePath(GL_FRAGMENT_SHADER)),
				(def_ForwardShader.GetShaderFilePath(GL_GEOMETRY_SHADER)),
			},
			//Deferred Rendering: GBuffer Shader
			{
				def_GBufferShader.GetName(),
				//REALLY BAD,
				(def_GBufferShader.GetShaderFilePath(GL_VERTEX_SHADER)),
				(def_GBufferShader.GetShaderFilePath(GL_FRAGMENT_SHADER)),
				(def_GBufferShader.GetShaderFilePath(GL_GEOMETRY_SHADER)),
			},
			//Deferred Rendering: Deferred Shader
			{
				def_DeferredShader.GetName(),
				//REALLY BAD,
				(def_DeferredShader.GetShaderFilePath(GL_VERTEX_SHADER)),
				(def_DeferredShader.GetShaderFilePath(GL_FRAGMENT_SHADER)),
				(def_DeferredShader.GetShaderFilePath(GL_GEOMETRY_SHADER)),
			},
		},
		//environment data
		{
			b_EnableSkybox,
			m_SkyboxInfluencity,
			m_SkyboxReflectivity,
			//skybox textures
			{
				def_Skybox.GetFacePaths()[0],
				def_Skybox.GetFacePaths()[1],
				def_Skybox.GetFacePaths()[2],
				def_Skybox.GetFacePaths()[3],
				def_Skybox.GetFacePaths()[4],
				def_Skybox.GetFacePaths()[5],
			},
			//skybox shader
			{
				def_SkyboxShader.GetName(),
				//REALLY BAD,
				(def_SkyboxShader.GetShaderFilePath(GL_VERTEX_SHADER)),
				(def_SkyboxShader.GetShaderFilePath(GL_FRAGMENT_SHADER)),
				(def_SkyboxShader.GetShaderFilePath(GL_GEOMETRY_SHADER)),
			},
		},
		//lights 
		{
			dirLightObject.dirlight, //directional Light
			m_PtLights,
			m_PtLightCount,
		},
		//shadow quality
		{
			//dir shadow 
			dirDepthMap.GetSize(),
			dirLightObject.sampleWorldPos,
			dirLightObject.dirLightShadow.config.cam_far,
			dirLightObject.dirLightShadow.config.cam_near,
			dirLightObject.cam_offset,
			dirLightObject.dirLightShadow.config.cam_size,
			//omni shadow
			//in might for generate value for all shadow
			m_PtDepthMapCubes[0].GetSize(),
			m_PtShadowConfig.cam_near,
			m_PtShadowConfig.cam_far,
			//REALLY BAD,
			(m_ShadowDepthShader.GetShaderFilePath(GL_VERTEX_SHADER)),
			(m_ShadowDepthShader.GetShaderFilePath(GL_FRAGMENT_SHADER)),
			(m_ShadowDepthShader.GetShaderFilePath(GL_GEOMETRY_SHADER)),
		},
	};
	SceneSerialiser::Instance().SerialiseScene("Assets/Scene/experiment.crrtscene", scene_info_data);
	
	//SceneSerialiser::Instance().SerialiseShader("Assets/Scene/shadowdepth.crrtshader", m_ShadowDepthShader);
}



void ForwardVsDeferredRenderingScene::BeginRenderScene()
{
	RenderCommand::ClearColour(m_ClearScreenColour);
	RenderCommand::Clear();
}

void ForwardVsDeferredRenderingScene::ShadowPass(Shader& depth_shader, const std::vector<std::weak_ptr<Entity>> renderable_meshes)
{
	m_FrameAsShadow = true;
	//need sorted data
	//material is not need only mesh geometry 
	dirDepthMap.Write();
	RenderCommand::CullFront();
	RenderCommand::ClearDepthOnly();//clear the depth buffer 
	//directional light
	depth_shader.Bind();
	m_ShadowDepthShader.SetUniform1i("u_IsOmnidir", 0);
	m_ShadowDepthShader.SetUniformMat4f("u_LightSpaceMat", dirLightObject.dirLightShadow.GetLightSpaceMatrix());
	//Draw Objects with material 
	//Renderer::DrawMesh(Mesh)


	
	for (const auto& e : renderable_meshes)
	{
		if (!e.expired())
		{
			auto entity = e.lock();
			if (entity->CanCastShadow())
			{
				depth_shader.SetUniformMat4f("u_Model", entity->GetWorldTransform());
				m_SceneRenderer.DrawMesh(entity->GetMesh());
			}
		}
	}

	dirDepthMap.UnBind();

	//point light shadows
	std::vector<glm::mat4> shadowMats = PointShadowCalculation::PointLightSpaceMatrix(m_PtLights[0].position, m_PtShadowConfig);
	//general shadowing values
	depth_shader.SetUniform1i("u_IsOmnidir", 1);
	depth_shader.SetUniform1f("u_FarPlane", m_PtShadowConfig.cam_far);

	
	for (unsigned int i = 0; i < m_PtLightCount; i++)
	{
		if (i > MAX_POINT_LIGHT_SHADOW)
			break;

		depth_shader.SetUniformVec3("u_LightPos", m_PtLights[i].position);
		shadowMats = PointShadowCalculation::PointLightSpaceMatrix(m_PtLights[i].position, m_PtShadowConfig);
		for (int f = 0; f < 6; ++f)
		{
			depth_shader.SetUniformMat4f(("u_ShadowMatrices[" + std::to_string(f) + "]").c_str(), shadowMats[f]);
		}

		m_PtDepthMapCubes[i].Write();//ready to write in the depth cube framebuffer for light "i"
		RenderCommand::ClearDepthOnly();//clear the depth buffer 

		//draw renderable meshes 
		for (const auto& e : renderable_meshes)
		{
			if (!e.expired())
			{
				auto entity = e.lock();
				if (entity->CanCastShadow())
				{
					depth_shader.SetUniformMat4f("u_Model", entity->GetWorldTransform());
					m_SceneRenderer.DrawMesh(entity->GetMesh());
				}
			}
		}
		//unbind current point light shadow cube
		m_PtDepthMapCubes[i].UnBind();
	}

	//done with shadow calculation
	depth_shader.UnBind();
	RenderCommand::CullBack();
	RenderCommand::Viewport(0, 0, window->GetWidth(), window->GetHeight());
}


void ForwardVsDeferredRenderingScene::OnSceneDebug()
{

	if (m_DebugPointLightRange)
	{
		for (const auto& pt : m_PtLights)
		{
			DebugGizmos::DrawWireSphere(pt.position, pt.CalculateLightRadius(0.02f), pt.colour);
		}
	}


	DebugGizmos::DrawSphere(m_PtOrbitOrigin, 5.0f);
	DebugGizmos::DrawWireThreeDisc(m_PtOrbitOrigin, m_SpawnZoneRadius);


	if (dirLightObject.dirLightShadow.debugPara)
	{
		auto& ds = dirLightObject.dirLightShadow;
		float dcv = dirLightObject.cam_offset + ds.config.cam_near * 0.5f; //dcv is the center/value between the near & far plane 
		glm::vec3 orthCamPos = dirLightObject.sampleWorldPos + (dirLightObject.dirlight.direction * dirLightObject.cam_offset);
		glm::vec3 farPlane = orthCamPos + (glm::normalize(-dirLightObject.dirlight.direction) * ds.config.cam_far);
		glm::vec3 nearPlane = orthCamPos + (glm::normalize(dirLightObject.dirlight.direction) * ds.config.cam_near);
		DebugGizmos::DrawOrthoCameraFrustrm(orthCamPos, dirLightObject.dirlight.direction,
			ds.config.cam_near, ds.config.cam_far, ds.config.cam_size,
			glm::vec3(0.0f, 1.0f, 0.0f));

		//Shadow Camera Sample Position 
		DebugGizmos::DrawCross(dirLightObject.sampleWorldPos);
	}



	if (m_PtShadowConfig.debugLight)
	{
		for (int i = 0; i < m_PtLightCount; i++)
		{
			if (m_EnableShadows)
			{
				DebugGizmos::DrawWireThreeDisc(m_PtLights[i].position, m_PtShadowConfig.cam_far, 10, m_PtLights[i].colour, 1.0f);
				DebugGizmos::DrawCross(m_PtLights[i].position);
			}

			//EXTRA 
			DebugGizmos::DrawLine(m_PtLights[i].position, m_PtOrbitOrigin, m_PtLights[i].colour);
			DebugGizmos::DrawSphere(m_PtLights[i].position, 1.0f, m_PtLights[i].colour);
		}
	}


}

void ForwardVsDeferredRenderingScene::ResetSceneFrame()
{
	m_FrameAsShadow = false;
}

void ForwardVsDeferredRenderingScene::ResizeBuffers(unsigned int width, unsigned int height)
{
	m_PrevViewWidth = width;
	m_PrevViewHeight = height;
	def_GBuffer.ResizeBuffer(width, height);
}


bool ForwardVsDeferredRenderingScene::AddPointLight(glm::vec3 pos, glm::vec3 col)
{
	if (m_PtLightCount > MAX_POINT_LIGHT)
		return false;
	
	m_PtLights.emplace_back(PointLight(pos, col));
	m_PtLights.back().enable = true;
	m_PtLights.back().attenuation[0] = 0.017f;
	m_PtLights.back().attenuation[1] = 0.022f;
	m_PtLights.back().attenuation[2] = 0.002f;

	m_PtLightCount++;
	return true;
}

void ForwardVsDeferredRenderingScene::MainUI()
{
	ImGui::Begin("Forward vs Deferred Rendering Scene");

	//////////////////////////////////////////
	// SCENE , CAMERA & SCENE PROPs
	//////////////////////////////////////////
	ImGui::SeparatorText("Window info");
	ImGui::Text("Win Title: %s", window->GetInitProp()->title);
	ImGui::Text("Window Width: %d", window->GetWidth());
	ImGui::Text("Window Height: %d", window->GetHeight());

	ImGui::Text("Init Width: %d", window->GetInitProp()->width);
	ImGui::Text("Init Height: %d", window->GetInitProp()->height);
	//CAMERA
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



	///////////////////////////////////////////
	// SCENE LIGHTS
	///////////////////////////////////////////
	ImGui::Spacing();
	ImGui::SeparatorText("Scene Properties");
	ImGui::ColorEdit3("clear Screen Colour", &m_ClearScreenColour[0]);
	ExternalMainUI_SceneDebugTreeNode();
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

	ExternalMainUI_LightTreeNode();


	ImGui::End();
}

void ForwardVsDeferredRenderingScene::ExternalMainUI_LightTreeNode()
{
	if (ImGui::TreeNode("Lights"))
	{

		//////////////////////////////////////
		// GENERAL LIGHT PROPS
		//////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Light Global Properties");
		ImGui::Checkbox("Enable Scene Shadow", &m_EnableShadows);

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
		if(ImGui::TreeNode("Directional Light"))
		{
			ImGui::Checkbox("Enable Directional", &dirLightObject.dirlight.enable);
			//ImGui::SameLine();
			//ImGui::Checkbox("Cast Shadow", &dirLightObject.dirlight.castShadow);
			ImGui::DragFloat3("Light Direction", &dirLightObject.dirlight.direction[0], 0.05f, -5.0f, 5.0f);
			//ImGui::DragFloat3("Light Direction", &dirLightObject.dirlight.direction[0], 0.1f, -1.0f, 1.0f);
			ImGui::ColorEdit3("Dir Light colour", &dirLightObject.dirlight.colour[0]);
			ImGui::SliderFloat("Light ambinentIntensity", &dirLightObject.dirlight.ambientIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Light diffuseIntensity", &dirLightObject.dirlight.diffuseIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Light specIntensity", &dirLightObject.dirlight.specularIntensity, 0.0f, 1.0f);
			if (dirLightObject.dirlight.castShadow)
			{
				if (ImGui::TreeNode("Shadow Camera Info"))
				{
					auto& shadow = dirLightObject.dirLightShadow;
					ImGui::SliderFloat("Camera Near", &shadow.config.cam_near, 0.0f, shadow.config.cam_far - 0.5f);
					ImGui::SliderFloat("Camera Far", &shadow.config.cam_far, shadow.config.cam_near + 0.5f, 1000.0f);
					ImGui::SliderFloat("Camera Size", &shadow.config.cam_size, 0.0f, 200.0f);
					ImGui::DragFloat3("Sample Pos", &dirLightObject.sampleWorldPos[0], 0.1f);
					ImGui::SliderFloat("Light Proj Offset", &dirLightObject.cam_offset, 0.0f, 100.0f);
					ImGui::Checkbox("Debug Dir Shadow Para", &shadow.debugPara);

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		
		
		//////////////////////////////////////////
		// Point Lights
		//////////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Point Lights");
		if (ImGui::TreeNode("Points Lights"))
		{
			ImGui::Spacing();
			ImGui::SeparatorText("Point Light Orbit");
			ImGui::DragFloat3("Orbit origin", &m_PtOrbitOrigin[0]);
			ImGui::SliderFloat("Desired Distance", &m_DesiredDistance, 0.0f, 500.0f);
			ImGui::SliderFloat("Desired Speed", &m_OrbitSpeed, -20.0f, 50.0f);

			if (ImGui::TreeNode("Shadow Camera Info"))
			{
				auto& shadow = m_PtShadowConfig;
				ImGui::Checkbox("Debug Pt Lights", &shadow.debugLight);
				ImGui::SliderFloat("Pt Shadow Camera Near", &shadow.cam_near, 0.0f, shadow.cam_far - 0.5f);
				ImGui::SliderFloat("Pt Shadow Camera Far", &shadow.cam_far, shadow.cam_near + 0.5f, 80.0f);
				ImGui::TreePop();
			}


			ImGui::Spacing();
			ImGui::SliderFloat("Spawn Zone", &m_SpawnZoneRadius, 0.0f, 500.0f);
			ImGui::Text("Current Point Light count: %d, MAX: %d", m_PtLightCount, MAX_POINT_LIGHT);
			if (ImGui::Button("Add Point Light"))
			{
				glm::vec3 pos = MathsHelper::RandomPointInSphere(m_SpawnZoneRadius) + m_PtOrbitOrigin;
				glm::vec3 col = MathsHelper::RandomColour();
				std::cout << "Adding Point Light.........\n";
				printf("At pos x: %f, y: %f, z: %f\n", pos.x, pos.y, pos.z);
				printf("At colour x: %f, y: %f, z: %f\n", col.x, col.y, col.z);
				if (!AddPointLight(pos, col))
					std::cout << "Failed to add light max reached!!!!\n";
				else
					printf("Successfully added a light at idx: %d\n", (m_PtLightCount - 1));
			}


			for (int i = 0; i < m_PtLightCount; i++)
			{
				if (i > MAX_POINT_LIGHT)
					break;

				std::string label = "point light: " + std::to_string(i);
				ImGui::SeparatorText(label.c_str());
				ImGui::Checkbox((label + " Enable light").c_str(), &m_PtLights[i].enable);

				ImGui::DragFloat3((label + " position").c_str(), &m_PtLights[i].position[0], 0.1f);

				ImGui::ColorEdit3((label + " colour").c_str(), &m_PtLights[i].colour[0]);
				ImGui::SliderFloat((label + " ambinentIntensity").c_str(), &m_PtLights[i].ambientIntensity, 0.0f, 1.0f);
				ImGui::SliderFloat((label + " diffuseIntensity").c_str(), &m_PtLights[i].diffuseIntensity, 0.0f, 1.0f);
				ImGui::SliderFloat((label + " specIntensity").c_str(), &m_PtLights[i].specularIntensity, 0.0f, 1.0f);
				ImGui::SliderFloat((label + " constant attenuation").c_str(), &m_PtLights[i].attenuation[0], 0.0f, 1.0f);
				ImGui::SliderFloat((label + " linear attenuation").c_str(), &m_PtLights[i].attenuation[1], 0.0f, 1.0f);
				ImGui::SliderFloat((label + " quadratic attenuation").c_str(), &m_PtLights[i].attenuation[2], 0.0f, 1.0f);
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void ForwardVsDeferredRenderingScene::ExternalMainUI_SceneDebugTreeNode()
{
	ImGui::Spacing();
	if (ImGui::TreeNode("Debug Scene"))
	{
		ImGui::Checkbox("Allow Scene Debug Gizmos", &m_DebugScene);
		if (m_DebugScene)
		{
			ImGui::Checkbox("Draw Point Light Range", &m_DebugPointLightRange);
			ImGui::Checkbox("Debug Dir Shadow Para", &dirLightObject.dirLightShadow.debugPara);
		}
		else
			ImGui::Text("Check Allow Scene Debug Gizmos");
		ImGui::TreePop();
	}
}

void ForwardVsDeferredRenderingScene::EnititiesUI()
{
	if (m_SceneEntities.size() > 0)
	{
		if (ImGui::Begin("Entities Debug UI"))
		{
			for (auto& e : m_SceneEntities)
				EntityDebugUI(*e);
		}
		ImGui::End();
	}
}

void ForwardVsDeferredRenderingScene::EntityDebugUI(Entity& entity)
{
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::PushID(entity.GetID());
	std::string label_name = entity.GetName();
	label_name += "ID: ";
	label_name += entity.GetID();
	ImGui::SeparatorText(label_name.c_str());
	ImGui::Checkbox("Can cast shadow", entity.CanCastShadowPtr());
	glm::vec3 translate, euler, scale;
	MathsHelper::DecomposeTransform(entity.GetTransform(), translate, euler, scale);
	bool update = ImGui::DragFloat3("Translate", &translate[0], 0.2f);
	update |= ImGui::DragFloat3("Rotation", &euler[0], 0.2f);
	update |= ImGui::DragFloat3("Scale", &scale[0], 0.2f);
	if (update)
	{
		entity.SetLocalTransform(glm::translate(glm::mat4(1.0f), translate) *
			glm::toMat4(glm::quat(glm::radians(euler))) *
			glm::scale(glm::mat4(1.0f), scale));

		if (entity.GetMaterial())
			b_ResortTransparency |= (entity.GetMaterial()->renderMode == CRRT_Mat::RenderingMode::Transparent);
	}
	if (entity.GetMaterial())
		ImGui::Text("Material Name: %s", entity.GetMaterial()->name);
	ImGui::Text("Number of Children: %d", entity.GetChildren().size());



	if (entity.GetChildren().size() > 0)
	{
		if (ImGui::TreeNode("Children"))
		{
			for (auto& e : entity.GetChildren())
				EntityDebugUI(*e);

			ImGui::TreePop();
		}
	}

	ImGui::PopID();
}

void ForwardVsDeferredRenderingScene::MaterialsUI()
{
	bool mat_ui = ImGui::Begin("Material");
	if (mat_ui)
	{
		ImGui::SeparatorText("Debuggig Model Material");

		for (auto& e : m_SceneEntities)
			EntityModelMaterial(*e);
	}


	ImGui::End();
}

void ForwardVsDeferredRenderingScene::EntityModelMaterial(const Entity& entity)
{
	auto& mat = entity.GetMaterial();
	if (mat)
	{
		static int blank_tex_id = blank_tex->GetID();
		int tex_id;
		ImGui::PushID(entity.GetID());
		ImGui::Text("1. %s, from mesh: %s", mat->name, entity.GetName());
		int curr_sel = (int)mat->renderMode;
		if (ImGui::Combo("Rendering Mode", &curr_sel, CRRT_Mat::GetAllRenderingModesAsName(), (int)CRRT_Mat::RenderingMode::Count))
		{
			mat->renderMode = (CRRT_Mat::RenderingMode)curr_sel;
			b_RebuildTransparency |= true;
		}
		ImGui::ColorEdit4("Colour", &mat->baseColour[0]);
		tex_id = (mat->baseMap) ? mat->baseMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Main Texture");
		tex_id = (mat->normalMap) ? mat->normalMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Normal Map");
		ImGui::Checkbox("Use Normal", &mat->useNormal);
		tex_id = (mat->specularMap) ? mat->specularMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Specular Map");
		tex_id = (mat->parallaxMap) ? mat->parallaxMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Parallax/Height Map");
		ImGui::Checkbox("Use Parallax", &mat->useParallax);
		ImGui::SliderFloat("Parallax/Height Scale", &mat->heightScale, 0.0f, 0.08f);
		ImGui::SliderInt("Shinness", &mat->shinness, 32, 256);
		ImGui::PopID();
	}

	for (size_t i = 0; i < entity.GetChildren().size(); i++)
		EntityModelMaterial(*entity.GetChildren()[i]);
}

void ForwardVsDeferredRenderingScene::GBufferDisplayUI()
{
	if (ImGui::Begin("Scene GBuffer"))
	{
		static int scale = 1;
		ImGui::SliderInt("image scale", &scale, 1, 5);
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

	}
	ImGui::End();
}


