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
	LoadSceneFromFile();
	//CreateEntities();
	//CreateLightDatas();
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


	OnRender();
}

void ForwardVsDeferredRenderingScene::OnRender()
{
	//Pre Rendering
	BeginRenderScene();
	PreUpdateGPUUniformBuffers(*m_Camera);
	//naive flat out self - children - children into list/collection, easy mutilple interation

	if (frames_count < 1)
	{
		for (const auto& e : m_SceneEntities)
			BuildRenderableMeshes(e);
	}

	//shadow data retrival 
	ShadowPass(m_ShadowDepthShader, m_RenderableEntities); //<----- important for storing shadow data in texture 2D & texture cube map probab;y move to renderer and data is sent back 

	//Start Rendering
	RenderCommand::Clear();
	//update uniform buffers with light current state data, e.t.c LightPass
	PostUpdateGPUUniformBuffers(); //<------- Not really necessary yet
	//Draw Skybox
	if (m_EnableSkybox)
		m_Skybox.Draw(m_SkyboxShader, m_SceneRenderer);
	//Render Opaques entities
	OpaquePass(m_ForwardShader, m_RenderableEntities);
	frames_count++;

	//return;
	SceneDebugger();
}

void ForwardVsDeferredRenderingScene::OnRenderUI()
{
	MainUI();
	EnititiesUI();
	MaterialsUI();
}

void ForwardVsDeferredRenderingScene::OnDestroy()
{
	if (m_PtLightCount > 0)
		SerialiseScene();
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
}

void ForwardVsDeferredRenderingScene::CreateEntities(const SceneData&  scene_data)
{
	//////////////////////////////////////
	// GENERATE SHADERS
	//////////////////////////////////////
	//model shader

	bool load_from_file = true;
	if (load_from_file)
	{
		auto& renderer_config = scene_data.m_RenderingConfig;
		ShaderFilePath shader_file_path
		{
			renderer_config.m_ForwardShader.m_Vertex,
			renderer_config.m_ForwardShader.m_Fragment,
			renderer_config.m_ForwardShader.m_Geometry,
		};
		m_ForwardShader.Create("model_forward_shading", shader_file_path);
		m_ForwardShader.Bind();
		m_ForwardShader.SetUniform1i("u_SkyboxMap", 4);


		auto& shadow_data = scene_data.m_Shadow;

		m_ShadowDepthShader.Create("point_shadow_depth", shadow_data.m_depthVerShader,
								shadow_data.m_depthFragShader, shadow_data.m_depthGeoShader);
	}


	auto& envir_data = scene_data.m_SceneEnvi;
	m_EnableSkybox = envir_data.m_EnableSkybox;
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
	m_Skybox.Create(def_skybox_faces);
	m_Skybox.ActivateMap(4);
	//skybox shading
	m_SkyboxShader.Create(envir_data.m_SkyboxShader.m_Name, envir_data.m_SkyboxShader.m_Vertex, 
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


	defaultFallBackMaterial = plainMat;

	////////////////////////////////////////
	// CREATE ENTITIES 
	////////////////////////////////////////

	//TEST TEST TEST TEST 


	glm::mat4 temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
						   glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
						   glm::scale(glm::mat4(1.0f), glm::vec3(50)); 

	//primitive construction
	std::shared_ptr<Mesh> m_QuadMesh = CRRT::PrimitiveMeshFactory::Instance().CreateQuad();
	std::shared_ptr<Mesh> cube_mesh = CRRT::PrimitiveMeshFactory::Instance().CreateCube();
	int id_idx = 0;

	//floor 
	Entity floor_plane_entity = Entity(id_idx++, "floor-plane-entity", temp_trans, m_QuadMesh, floorMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(floor_plane_entity));

	//move up 
	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.6f, 0.0f));
	Entity cube_entity = Entity(id_idx++, "cube-entity", temp_trans, cube_mesh, plainMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(cube_entity));

	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.5f, 0.0f));
	Entity sphere_entity = Entity(id_idx++, "sphere-entity", temp_trans, CRRT::PrimitiveMeshFactory::Instance().CreateSphere(), plainMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(sphere_entity));

	//testing sponza
	temp_trans = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
				 glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
	std::shared_ptr<Entity> sponza_model = m_NewModelLoader.LoadAsEntity(FilePaths::Instance().GetPath("sponza"), true);
	sponza_model->SetLocalTransform(temp_trans);
	m_SceneEntities.emplace_back(sponza_model);




}
void ForwardVsDeferredRenderingScene::CreateGPUDatas()
{
	////////////////////////////////////////
	// UNIFORM BUFFERs
	////////////////////////////////////////
	//------------------Camera Matrix Data UBO-----------------------------/
	long long int buf_size = 2 * sizeof(glm::mat4);// +sizeof(glm::vec2);   //to store view, projection
	m_CamMatUBO.Generate(buf_size);
	m_CamMatUBO.BindBufferRndIdx(0, buf_size, 0);

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
	m_ForwardShader.Bind();
	m_ForwardShader.SetUniformBlockIdx("u_CameraMat", 0);
	m_ForwardShader.SetUniformBlockIdx("u_LightBuffer", 1);
	m_ForwardShader.SetUniformBlockIdx("u_EnvironmentBuffer", 2);

	m_SkyboxShader.Bind();
	m_SkyboxShader.SetUniformBlockIdx("u_CameraMat", 0);
}

void ForwardVsDeferredRenderingScene::LoadSceneFromFile()
{
	//--------------------Light--------------------------------/
	//Dir light
	auto& dl = dirLightObject.dirlight;

	SceneData scene_data{};
	if (!SceneSerialiser::Instance().LoadScene("Assets/Scene/experiment.crrtscene", scene_data))
		printf("[SCENE FILE LOADING]: Failed to retrive all data from file!!!!!!!");

	dl = scene_data.m_Light.dir_Light;

	m_PtLightCount = scene_data.m_Light.m_PtLightCount;
	//m_PtLights[0] = *s_lt_data.ptLights;

	if (m_PtLightCount > 0)
		std::memcpy(m_PtLights, scene_data.m_Light.ptLights, m_PtLightCount * sizeof(PointLight));

	//shadow map
	auto& shadow_data = scene_data.m_Shadow;
	dirDepthMap.Generate(shadow_data.m_DirDepthResQuality);
	dirLightObject.dirLightShadow.config.cam_far = shadow_data.m_DirZFar;
	dirLightObject.sampleWorldPos = shadow_data.m_DirSamplePos;
	dirLightObject.dirLightShadow.config.cam_far = shadow_data.m_DirZFar;
	dirLightObject.cam_offset = shadow_data.m_DirSampleOffset;
	dirLightObject.dirLightShadow.config.cam_size = shadow_data.m_DirOrthoSize;


	//point light shadow map 
	for (auto& pt : m_PtLights)
	{
		//using push_back instead of emplace_back 
		//to create a copy when storing in vector 
		m_PtDepthMapCubes.push_back(ShadowCube(shadow_data.m_PtDepthResQuality));
		m_PtDepthMapCubes.back().Generate();
	}



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


	//Load entities
	CreateEntities(scene_data);
}

void ForwardVsDeferredRenderingScene::SerialiseScene()
{
	//experiment with loading data
	SceneData scene_info_data
	{
		"Forward Vs Deferred Scene",
		true,	//m_HasLight;
		true,	//m_HasShadow = true;
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
				m_ForwardShader.GetName(),
				//REALLY BAD,
				(m_ForwardShader.GetShaderFilePath(GL_VERTEX_SHADER)),
				(m_ForwardShader.GetShaderFilePath(GL_FRAGMENT_SHADER)),
				(m_ForwardShader.GetShaderFilePath(GL_GEOMETRY_SHADER)),
			},
		},
		//environment data
		{
			m_EnableSkybox,
			m_SkyboxInfluencity,
			m_SkyboxReflectivity,
			//skybox textures
			{
				m_Skybox.GetFacePaths()[0],
				m_Skybox.GetFacePaths()[1],
				m_Skybox.GetFacePaths()[2],
				m_Skybox.GetFacePaths()[3],
				m_Skybox.GetFacePaths()[4],
				m_Skybox.GetFacePaths()[5],
			},
			//skybox shader
			{
				m_SkyboxShader.GetName(),
				//REALLY BAD,
				(m_SkyboxShader.GetShaderFilePath(GL_VERTEX_SHADER)),
				(m_SkyboxShader.GetShaderFilePath(GL_FRAGMENT_SHADER)),
				(m_SkyboxShader.GetShaderFilePath(GL_GEOMETRY_SHADER)),
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

void ForwardVsDeferredRenderingScene::PreUpdateGPUUniformBuffers(Camera& cam)
{
	//------------------Camera Matrix Data UBO-----------------------------/
	m_CamMatUBO.SetSubDataByID(&(cam.CalculateProjMatrix(window->GetAspectRatio())[0][0]), sizeof(glm::mat4), 0);
	m_CamMatUBO.SetSubDataByID(&(cam.CalViewMat()[0][0]), sizeof(glm::mat4), sizeof(glm::mat4));
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
		if (i > MAX_POINT_LIGHT)
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

void ForwardVsDeferredRenderingScene::BuildRenderableMeshes(const std::shared_ptr<Entity>& entity)
{	
	auto& renderable_mesh = entity->GetMesh();

	if (renderable_mesh)
		m_RenderableEntities.emplace_back(entity);

	auto& children = entity->GetChildren();
	for (auto& c : children)
		BuildRenderableMeshes(c);
}

void ForwardVsDeferredRenderingScene::PostUpdateGPUUniformBuffers()
{
	unsigned int offset_pointer = 0;
	offset_pointer = 0;
	dirLightObject.dirlight.UpdateUniformBufferData(m_LightDataUBO, offset_pointer);
	//for (int i = 0; i < MAX_POINT_LIGHT; i++)
		//ptLight[i].UpdateUniformBufferData(m_LightDataUBO, offset_pointer);

	//direction needs to be updated before point light 
	//has point light is dynamic, but the max light data are updated every frame
	//if only one light is available, we only update the available light 
	//and perform directional light before keeps the Light uniform buffer integrity 
	for (int i = 0; i < m_PtLightCount; i++)
		if (i < MAX_POINT_LIGHT)
			m_PtLights[i].UpdateUniformBufferData(m_LightDataUBO, offset_pointer);


	m_ForwardShader.Bind();
	m_ForwardShader.SetUniformVec3("u_ViewPos", m_Camera->GetPosition());

	//m_ForwardShader.SetUniform1i("u_EnableSceneShadow", m_EnableShadows);
	if (m_EnableShadows && m_FrameAsShadow)
	{
		m_ForwardShader.SetUniformMat4f("u_DirLightSpaceMatrix", dirLightObject.dirLightShadow.GetLightSpaceMatrix());
		//tex unit 0 >> texture (base map)
		//tex unit 1 >> potenially normal map
		//tex unit 2 >> potenially parallax map
		//tex unit 3 >> shadow map (dir Light)
		//tex unit 4 >> skybox cube map
		//tex unit 5 >> shadow cube (pt Light)
		dirDepthMap.Read(3);
		m_ForwardShader.SetUniform1i("u_DirShadowMap", 3);

		//point light shadow starts at 5 
		for (int i = 0; i < m_PtLightCount; i++)
		{
			m_PtDepthMapCubes[i].Read(5 + i);
			m_ForwardShader.SetUniform1i(("u_PointShadowCubes[" + std::to_string(i) + "]").c_str(), 5 + i);
		}
	}


	m_ForwardShader.SetUniform1i("u_PtLightCount", m_PtLightCount);
	m_ForwardShader.SetUniform1i("u_SceneAsShadow", m_FrameAsShadow);
	//environment
	offset_pointer = 0;
	m_EnviUBO.SetSubDataByID(&m_EnableSkybox, sizeof(bool), offset_pointer);
	offset_pointer += sizeof(int);
	m_EnviUBO.SetSubDataByID(&m_SkyboxInfluencity, sizeof(float), offset_pointer);
	offset_pointer += sizeof(float);
	m_EnviUBO.SetSubDataByID(&m_SkyboxReflectivity, sizeof(float), offset_pointer);
	//m_SceneShader.SetUniform1i("u_SkyboxMap", 4);
}

void ForwardVsDeferredRenderingScene::OpaquePass(Shader& main_shader, const std::vector<std::weak_ptr<Entity>> opaque_entities)
{
	main_shader.Bind();
	for (const auto& e : opaque_entities)
	{
		if (!e.expired())
		{
			auto& mesh = e.lock()->GetMesh();
			auto& mat = e.lock()->GetMaterial();


			if (mesh)
			{
				if (mat)
					MaterialShaderBindHelper(*mat, main_shader);
				else//if no material, use first scene mat as default
					MaterialShaderBindHelper(*defaultFallBackMaterial, main_shader);


				main_shader.SetUniformMat4f("u_Model", e.lock()->GetWorldTransform());
				m_SceneRenderer.DrawMesh(mesh);
			}
		}
	}
}

void ForwardVsDeferredRenderingScene::SceneDebugger()
{
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
			DebugGizmos::DrawWireThreeDisc(m_PtLights[i].position, m_PtShadowConfig.cam_far, 10, m_PtLights[i].colour, 1.0f);
			DebugGizmos::DrawCross(m_PtLights[i].position);
		}
	}


}

void ForwardVsDeferredRenderingScene::ResetSceneFrame()
{
	m_FrameAsShadow = false;
}

void ForwardVsDeferredRenderingScene::MaterialShaderBindHelper(Material& mat, Shader& shader)
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
	//if (mat.parallaxMap)
	//{
	//	mat.parallaxMap->Activate(tex_units);
	//	shader.SetUniform1i("u_Material.parallaxMap", tex_units++);
	//}
	//shader.SetUniform1i("u_Material.isTransparent", (mat.renderMode == CRRT_Mat::RenderingMode::Transparent));
	shader.SetUniform1i("u_Material.useNormal", mat.useNormal && mat.normalMap);
	shader.SetUniform1i("u_Material.shinness", mat.shinness);
	//shader.SetUniform1i("u_Material.useParallax", mat.useParallax);
	//shader.SetUniform1f("u_Material.parallax", mat.heightScale);
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
		ImGui::SliderFloat("Move Speed", m_Camera->Ptr_MoveSpeed(), 5.0f, 50.0f);
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
	ExternalMainUI_LightTreeNode();


	ImGui::End();
}

void ForwardVsDeferredRenderingScene::ExternalMainUI_LightTreeNode()
{
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::SeparatorText("Environment");
		ImGui::Checkbox("Use Skybox", &m_EnableSkybox);
		ImGui::SliderFloat("Skybox influencity", &m_SkyboxInfluencity, 0.0f, 1.0f);
		ImGui::SliderFloat("Skybox reflectivity", &m_SkyboxReflectivity, 0.0f, 1.0f);

		ImGui::Spacing();
		ImGui::SeparatorText("Light Global Properties");
		ImGui::Checkbox("Enable Scene Shadow", &m_EnableShadows);


		//////////////////////////////////////
		// Directional Light
		//////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Directional Light");
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
		ImGui::Spacing();

		//////////////////////////////////////////
		// Point Lights
		//////////////////////////////////////////
		ImGui::SeparatorText("Point Lights");
		if (ImGui::TreeNode("Points Lights"))
		{
			//ImGui::Checkbox("Point Light Gizmos", &ptLightGizmos);
			if (ImGui::TreeNode("Shadow Camera Info"))
			{
				auto& shadow = m_PtShadowConfig;
				ImGui::Checkbox("Debug Pt Lights", &shadow.debugLight);
				ImGui::SliderFloat("Pt Shadow Camera Near", &shadow.cam_near, 0.0f, shadow.cam_far - 0.5f);
				ImGui::SliderFloat("Pt Shadow Camera Far", &shadow.cam_far, shadow.cam_near + 0.5f, 80.0f);
				ImGui::TreePop();
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

void ForwardVsDeferredRenderingScene::EnititiesUI()
{
	if (m_SceneEntities.size() > 0)
	{
		ImGui::Begin("Entities Debug UI");
		for (auto& e : m_SceneEntities)
			EntityDebugUI(*e);
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
			mat->renderMode = (CRRT_Mat::RenderingMode)curr_sel;
		ImGui::ColorEdit4("Colour", &mat->baseColour[0]);
		tex_id = (mat->baseMap) ? mat->baseMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Main Texture");
		tex_id = (mat->normalMap) ? mat->normalMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Normal Map");
		ImGui::Checkbox("Use Normal", &mat->useNormal);
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