#include "ExperimentScene.h"

#include "Renderer/RenderCommand.h"

#include "Renderer/Material.h"
#include "Util/FilePaths.h"

#include "Renderer/Meshes/Mesh.h"
#include "Scene Graph/Entity.h"

#include "Renderer/DebugGizmos.h"

#include "External Libs/imgui/imgui.h"
#include "Util/MathsHelpers.h"
#include <glm/gtx/quaternion.hpp>

#include "Util/GPUStructure.h"

#include "Renderer/Meshes/PrimitiveMeshFactory.h"

#include "Util/GameTime.h"
#include <iostream>

#include "Geometry/Frustum.h"

/// <summary>
/// Need to remove this later
/// </summary>
/// <param name="window"></param>
void ExperimentScene::SetWindow(Window* window)
{
	this->window = window;
}

void ExperimentScene::OnInit(Window* window)
{
	Scene::OnInit(window);
	window->UpdateProgramTitle("ReWriting Scene");

	//Need to move out 
	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 1.0f/*0.5f*/);


	InitRenderer();
	CreateEntities();
	CreateGPUDatas();
	CreateLightDatas();
}

void ExperimentScene::OnUpdate(float delta_time)
{

	//definately need to move this out
	if (enableSceneShadow)
	{
		dirLightObject.dirLightShadow.UpdateProjMat();
		dirLightObject.dirLightShadow.UpdateViewMatrix(dirLightObject.sampleWorldPos,
			dirLightObject.dirlight.direction,
			dirLightObject.cam_offset);
	}


	OnRender();
}

void ExperimentScene::OnRender()
{
	//Pre Rendering
	BeginRenderScene();
	PreUpdateGPUUniformBuffers(*m_Camera);
	BuildSceneEntities(); //<------Not implemented yet
	ShadowPass(m_ShadowDepthShader); //<----- important for storing shadow data in texture 2D & texture cube map probab;y move to renderer and data is sent back 

	//Start Rendering
	RenderCommand::Clear();
	//post is also used for all shader view & shadow data but move to appropiate ubo later
	PostUpdateGPUUniformBuffers(); //<------- Not really necessary yet
	/////////////////////////////
	// First Render Pass
	/////////////////////////////
	m_SceneScreenFBO.Bind();
	RenderCommand::Clear();
	DrawScene(m_SceneShader);
	SceneDebugger();
	m_SceneScreenFBO.UnBind();


	//Post Rendering (post Process) 
	PostProcess();

	//////////////////////////////
	// EXPERIMENT WC MRT GBUFFER
	//////////////////////////////
	m_TestGBuffer.Bind();
	RenderCommand::Clear();
	DrawScene(m_TestGBufferShader);
	m_TestGBuffer.UnBind();



	//After Rendering (Clean-up/Miscellenous)
	//Change the camera buffer with top down cam
	PreUpdateGPUUniformBuffers(m_TopDownCamera);
	m_TopDownFBO.Bind();						//bind fbo
	RenderCommand::Clear();						
	//draw scene in buffer
	DrawScene(m_SceneShader);
	SceneDebugger();
	m_TopDownFBO.UnBind();

	if (m_PrevViewWidth != window->GetWidth() || m_PrevViewHeight != window->GetHeight())
		ResizeBuffers(window->GetWidth(), window->GetHeight());

}

void ExperimentScene::OnRenderUI()
{
	MainUI();
	EnititiesUI();
	MaterialsUI();
	EditTopViewUI();
	TestMRT_GBufferUI();
}

void ExperimentScene::OnDestroy()
{
	//Perform all required clean up here
}

void ExperimentScene::InitRenderer()
{
	RenderCommand::EnableDepthTest();
	RenderCommand::EnableFaceCull();

	RenderCommand::CullBack();

	RenderCommand::DepthTestMode(DepthMode::LEEQUAL);
	//Other render behaivour / Data
	//Resolution setting 
	//Shadow config modification
	//etc 

	m_PrevViewWidth = window->GetWidth();
	m_PrevViewHeight = window->GetHeight();

	m_SceneScreenFBO.Generate(window->GetWidth(), window->GetHeight());

	m_TopDownFBO.Generate(window->GetWidth(), window->GetHeight());
	m_TopDownCamera = Camera(glm::vec3(0.0f, 62.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f, -90.0f, 0.0f, 0.0f);

	//Post Process shader
	ShaderFilePath screen_shader_file_path
	{
		//"Assets/Shaders/Learning/MSAA/AA_PostProcess_Vertex.glsl",
		"Assets/Shaders/Learning/Post Process/QuadScreenVertex.glsl",
		"Assets/Shaders/Learning/Post Process/PostProcessHDRfrag.glsl"
	};
	m_PostImgShader.Create("post render shader", screen_shader_file_path);
}

void ExperimentScene::CreateEntities()
{
	///////////////////////////////////////////////////////////////////////
	// SKY BOX: Cube Texture Map
	///////////////////////////////////////////////////////////////////////
	std::vector<std::string> def_skybox_faces
	{
		"Assets/Textures/Skyboxes/default_skybox/right.jpg",
		"Assets/Textures/Skyboxes/default_skybox/left.jpg",
		"Assets/Textures/Skyboxes/default_skybox/top.jpg",
		"Assets/Textures/Skyboxes/default_skybox/bottom.jpg",
		"Assets/Textures/Skyboxes/default_skybox/front.jpg",
		"Assets/Textures/Skyboxes/default_skybox/back.jpg"
	};
	m_Skybox.Create(def_skybox_faces);
	m_Skybox.ActivateMap(5);

	//////////////////////////////////////
	// GENERATE SHADERS
	//////////////////////////////////////
	//model shader
	ShaderFilePath shader_file_path
	{
		"Assets/Shaders/StandardVertex.glsl", //vertex shader
		"Assets/Shaders/StandardFrag.glsl", //fragment shader
	};
	m_SceneShader.Create("model_shader", shader_file_path);
	m_SceneShader.Bind();
	m_SceneShader.SetUniform1i("u_SkyboxMap", 5);
	ShaderFilePath point_shadow_shader_file_path
	{
		"Assets/Shaders/ShadowMapping/ShadowDepthVertex.glsl", //vertex shader
		"Assets/Shaders/ShadowMapping/ShadowDepthFrag.glsl", //fragment shader
		"Assets/Shaders/ShadowMapping/ShadowDepthGeometry.glsl", //geometry shader
	};

	m_ShadowDepthShader.Create("point_shadow_depth", point_shadow_shader_file_path);

	ShaderFilePath skybox_shader_file_path{
	"Assets/Shaders/Utilities/Skybox/SkyboxVertex.glsl",
	"Assets/Shaders/Utilities/Skybox/SkyboxFragment.glsl" };

	m_SkyboxShader.Create("skybox_shader", skybox_shader_file_path);


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

	auto glass2Mat = std::make_shared<Material>();
	glass2Mat->name = "Glass Material";
	glass2Mat->baseMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("glass"));


	m_SceneMaterials.emplace_back(plainMat);
	m_SceneMaterials.emplace_back(floorMat);


	////////////////////////////////////////
	// CREATE ENTITIES 
	////////////////////////////////////////

	//TEST TEST TEST TEST 


	glm::mat4 temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 10.0f)) *
						   glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 1.0f));

	//primitive construction
	m_QuadMesh = CRRT::PrimitiveMeshFactory::Instance().CreateQuad();
	std::shared_ptr<Mesh> cube_mesh = CRRT::PrimitiveMeshFactory::Instance().CreateCube();
	//cube_mesh.Create();
	int id_idx = 0;
	//id_idx = newModel->GetID() + 1;

	Entity transparent_1 = Entity(id_idx++, "transparent_1_entity", temp_trans, cube_mesh, glassMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(transparent_1));
	temp_trans = glm::translate(temp_trans, glm::vec3(0.0f, 0.0f, 10.0f));
	Entity transparent_2 = Entity(id_idx++, "transparent_2_entity", temp_trans, cube_mesh, glass2Mat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(transparent_2));
	temp_trans = glm::translate(temp_trans, glm::vec3(0.0f, 0.5f, -5.0f)) * 
				 glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.1f, 10.0f));
	Entity transparent_3 = Entity(id_idx++, "transparent_3_entity", temp_trans, cube_mesh, glassMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(transparent_3));
	
	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
				 glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
				 glm::scale(glm::mat4(1.0f), glm::vec3(50)); 

	//floor 
	Entity floor_plane_entity = Entity(id_idx++, "floor-plane-entity", temp_trans, m_QuadMesh, floorMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(floor_plane_entity));

	//move up 
	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.6f, 0.0f));
	Entity cube_entity = Entity(id_idx++, "cube-entity", temp_trans, cube_mesh, plainMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(cube_entity));
	//move right
	temp_trans = glm::translate(temp_trans, glm::vec3(5.0f, 0.0f, 0.0f)) * 
				 glm::scale(temp_trans, glm::vec3(2.0f));
	Entity cube1_entity = Entity(id_idx++, "cube1-entity", temp_trans, cube_mesh, plainMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(cube1_entity));
	//move up & add to previous as world child & scale down
	temp_trans = glm::translate(temp_trans, glm::vec3(0.0f, 2.0f, 0.0f)) *
				 glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	Entity cube1child_entity = Entity(id_idx++, "cube1child-entity", temp_trans, cube_mesh, plainMat);
	m_SceneEntities.back()->AddWorldChild(cube1child_entity);

	//create  a sphere with world pos an center
	//then add as cube1child_entity child
	Mesh sphere_mesh;
	sphere_mesh = CRRT::PrimitiveMeshFactory::Instance().CreateASphere();
	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.5f, 0.0f));// *
				// glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	Entity sphere_entity = Entity(id_idx++, "sphere-entity", temp_trans, CRRT::PrimitiveMeshFactory::Instance().CreateSphere(), plainMat);
	//cube1child_entity.AddWorldChild(sphere_entity);
	//Quick Hack
	m_SceneEntities.back()->GetChildren()[0]->AddWorldChild(sphere_entity);

	unsigned int hierarchy_count = 0; // 500;
	std::string name = "child_cube";
	temp_trans = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 5.0f, 5.0f));
	std::shared_ptr<Entity> prev = m_SceneEntities.back()->GetChildren()[0];
	for (unsigned int i = 0; i < hierarchy_count; i++)
	{
		std::shared_ptr<Entity> child_cube = std::make_shared<Entity>(id_idx++, (name + std::to_string(i)), temp_trans, cube_mesh, plainMat);
		prev->AddLocalChild(child_cube);
		prev = child_cube;
	}


	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(-14.0f, 13.0f, -20.0f)) * 
							glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
							glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	//model 1
	std::shared_ptr<Entity> newModel2 = m_NewModelLoader.LoadAsEntity(FilePaths::Instance().GetPath("electrical-charger"), true);
	newModel2->SetLocalTransform(temp_trans);
	m_SceneEntities.emplace_back(newModel2);

	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 2.0f, 0.0f)) * 
						   glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
						   glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	//model 2 
	std::shared_ptr<Entity> newModel = m_NewModelLoader.LoadAsEntity(FilePaths::Instance().GetPath("shapes"), true);
	newModel->SetLocalTransform(temp_trans);
	m_SceneEntities.emplace_back(newModel);


	std::shared_ptr<Entity> bunny = m_NewModelLoader.LoadAsEntity(FilePaths::Instance().GetPath("bunny"), true);
	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.8f, -17.6f)) *
					glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	bunny->SetLocalTransform(temp_trans);
	m_SceneEntities.emplace_back(bunny);




	///////////////////////////////////////
	// EXPERIMENTING FOR GBUFFER
	///////////////////////////////////////
	m_TestGBuffer.Generate(window->GetWidth(), window->GetHeight(), 3);
	ShaderFilePath test_shader_file_path
	{
		"Assets/Shaders/Experimental/BasicVertexPos.glsl", //vertex shader
		"Assets/Shaders/Experimental/ExperimentGBuffer.glsl", //fragment shader
	};
	m_TestGBufferShader.Create("experiment_GBuffer_shader", test_shader_file_path);
}

/// <summary>
/// Issue need to know if any shader needs UBO
/// </summary>
void ExperimentScene::CreateGPUDatas()
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
	m_LightDataUBO.Generate(light_buffer_size);
	m_LightDataUBO.BindBufferRndIdx(1, light_buffer_size, 0);


	
	//------------------Enviroment Data UBO-----------------------------/
	long long int envi_buffer_size = CRRT::EnvironmentData::GetGPUSize();
	m_EnviUBO.Generate(envi_buffer_size);
	m_EnviUBO.BindBufferRndIdx(2, envi_buffer_size, 0);

	//Assign UBO, if necessary 
	m_SceneShader.Bind();
	m_SceneShader.SetUniformBlockIdx("u_CameraMat", 0);
	m_SceneShader.SetUniformBlockIdx("u_LightBuffer", 1);
	m_SceneShader.SetUniformBlockIdx("u_EnvironmentBuffer", 2);

	m_SkyboxShader.Bind();
	m_SkyboxShader.SetUniformBlockIdx("u_CameraMat", 0);
}

void ExperimentScene::CreateLightDatas()
{
	//--------------------Light--------------------------------/
	//Dir light
	auto& dl = dirLightObject.dirlight;
	dirLightObject.sampleWorldPos = glm::vec3(5.0f, 2.0f, -10.0f);
	dirLightObject.cam_offset = 10.0f;
	dl.ambientIntensity = 0.05f;
	dl.diffuseIntensity = 0.4f;
	dl.specularIntensity = 0.2f;
	dl.colour = glm::vec3(1.0f, 0.9568f, 0.8392f);
	dl.enable = true;
	dl.castShadow = true;
	dl.direction = glm::vec3(-1.0f, 1.0f, -1.0f);


	//shadow map
	dirDepthMap.Generate(2048, 2048);
	dirLightObject.dirLightShadow.config.cam_far = 70.0f;
}




//------------------------------------Probably move some later------------------------------------\



void ExperimentScene::BeginRenderScene()
{
	RenderCommand::ClearColour(m_ClearScreenColour);
	RenderCommand::Clear();
}

void ExperimentScene::PreUpdateGPUUniformBuffers(Camera& cam)
{
	//------------------Camera Matrix Data UBO-----------------------------/
	m_CamMatUBO.SetSubDataByID(&(cam.CalculateProjMatrix(window->GetAspectRatio())[0][0]), sizeof(glm::mat4), 0);
	m_CamMatUBO.SetSubDataByID(&(cam.CalViewMat()[0][0]), sizeof(glm::mat4), sizeof(glm::mat4));

}


/// <summary>
/// Use this to build scene based on visibility 
/// etc
/// </summary>
void ExperimentScene::BuildSceneEntities()
{
	//before build clean previous list 
	m_SceneEntitiesWcRenderableMesh.clear();
	m_TransparentEntites.clear();
	m_OpaqueEntites.clear();

	//later build by frstrum view


	// entity sorting / scene building 
	// 
	//	by mesh data
	// 
	bool build_only_visible = true;
	std::vector<std::weak_ptr<Entity>> temp_entities;
	temp_entities.reserve(m_SceneEntities.size());
	if (build_only_visible)
	{
		temp_entities = BuildVisibleEntities(m_SceneEntities);
	}
	else
	{
		for (const auto& e : m_SceneEntities)
			temp_entities.emplace_back(e);
	}

	//for (auto& e : m_SceneEntities)
	for (auto& e : temp_entities)
		BuildEntitiesWithRenderMesh(e);

	//	by material
	//	by transparency / opacity
	//only enitities with material would be rendered
	BuildOpacityTransparencyFromRenderMesh(m_SceneEntitiesWcRenderableMesh);
	//for (auto& e : m_SceneEntities)
	//	BuildSceneEntitiesViaOpacityTransparency(e);
	
	// 
	//  by distance 
	//sort transparent entities by distance
	SortByViewDistance(m_TransparentEntites);

	//  by frustrum occulsion 
	//	by shadowcast
}



std::vector<std::weak_ptr<Entity>> ExperimentScene::BuildVisibleEntities(const std::vector<std::shared_ptr<Entity>>& entity_collection)
{
	//return std::vector<std::weak_ptr<Entity>> instead of std::vector<std::weak_ptr<Entity>>& 
	// because by return a ref the visible_entity is create within the function on stack and as soon as the function goes out of scope
	// the variable gets deleted and returns an empty vector. but by returning a copy this ensures a returned data.
	//loop through all entities 
	//and create an aabb for entities
	//then check aabb visiblity, if not remove entity

	//create a frustum for the current camera state 
	const auto& cam = m_Camera;
	Frustum frustum = Frustum(cam->GetPosition(), cam->GetForward(),cam->GetUp(), *cam->Ptr_Near(), 
							 *cam->Ptr_Far(), *cam->Ptr_FOV(), window->GetAspectRatio());

	std::vector<std::weak_ptr<Entity>> visible_entity;
	visible_entity.reserve(entity_collection.size());

	//loop all entities
	for (const auto& e : entity_collection)
	{
		AABB bounds;
		if (e->GetMesh())
		{
			//get entity mesh AABB
			bounds = e->GetMesh()->GetAABB();
			//transform aabb, based on enitity transformation
			bounds = bounds.Tranformed(e->GetWorldTransform());
		}
		//if e entity has children encapsulate their AABB
		if (e->GetChildren().size() > 0)
			bounds = e->GetEncapsulatedChildrenAABB(); //already computed

		//check if aabb bounds is in frustum 
		//if (frustum.PointsInFrustum(bounds))
		if (frustum.IsAABBVisible(bounds))
			visible_entity.emplace_back(e); //store if entity is visible
	}

	//return all visiblities
	return visible_entity;
}

/// <summary>
/// Recursively build scene with renderable mesh as only enitities with mesh could be rendered.
/// </summary>
/// <param name="parent_entity"></param>
void ExperimentScene::BuildEntitiesWithRenderMesh(const std::shared_ptr<Entity>& parent_entity)
{
	//this is wrong
	//auto& render_mesh = parent_entity.lock()->GetMaterial();
	//need to be GetMesh
	auto& render_mesh = parent_entity->GetMesh();

	if (render_mesh)
		m_SceneEntitiesWcRenderableMesh.emplace_back(parent_entity);

	auto& children = parent_entity->GetChildren();
	for (auto c = children.begin(); c != children.end(); ++c)
		BuildEntitiesWithRenderMesh(*c);
}

void ExperimentScene::BuildEntitiesWithRenderMesh(const std::weak_ptr<Entity>& parent_entity)
{

	if (!parent_entity.expired())
	{
		//this is wrong
		//auto& render_mesh = parent_entity.lock()->GetMaterial();
		//need to be GetMesh
		auto& render_mesh = parent_entity.lock()->GetMesh();

		if (render_mesh)
			m_SceneEntitiesWcRenderableMesh.emplace_back(parent_entity);

		auto& children = parent_entity.lock()->GetChildren();
		for (auto c = children.begin(); c != children.end(); ++c)
			BuildEntitiesWithRenderMesh(*c);
	}
}

void ExperimentScene::BuildOpacityTransparencyFromRenderMesh(const std::vector<std::weak_ptr<Entity>>& renderable_list)
{
	for (const auto& e : renderable_list)
	{
		if (!e.expired())
		{
			//auto& mat = e.lock()->GetMaterial();
			std::shared_ptr<Material> mat = e.lock()->GetMaterial();

			//if mesh, does not have a material use default
			if (!mat)
				mat = m_SceneMaterials[0];


			switch (mat->renderMode)
			{
				case CRRT_Mat::RenderingMode::Transparent:
					m_TransparentEntites.emplace_back(e);
					break;
				case CRRT_Mat::RenderingMode::Opaque:
					m_OpaqueEntites.emplace_back(e);
					break;
				default:
					m_OpaqueEntites.emplace_back(e);
					break;
			}
		}
	}
}


/// <summary>
/// NOT IDEAL TO BUILD SCENE WITH
/// Issue with this is that the material is sorted not enititymesh
/// </summary>
/// <param name="parent_entity"></param>
void ExperimentScene::BuildSceneEntitiesViaOpacityTransparency(const std::shared_ptr<Entity>& parent_entity)
{
	auto& mat = parent_entity->GetMaterial();
	if (mat)
	{
		switch (mat->renderMode)
		{
			case CRRT_Mat::RenderingMode::Transparent:
				m_TransparentEntites.emplace_back(parent_entity);
				break;
			case CRRT_Mat::RenderingMode::Opaque:
				m_OpaqueEntites.emplace_back(parent_entity);
				break;
			default:
				m_OpaqueEntites.emplace_back(parent_entity);
				break;
		}
	}

	auto& children = parent_entity->GetChildren();

	for (auto c = children.begin(); c != children.end(); ++c)
		BuildSceneEntitiesViaOpacityTransparency(*c);
}

void ExperimentScene::SortByViewDistance(std::vector<std::weak_ptr<Entity>>& sorting_list)
{
	//probably move later
	for (const auto& e : sorting_list)
		e.lock()->UpdateViewSqrDist(m_Camera->GetPosition());

	std::sort(sorting_list.begin(), sorting_list.end(), Entity::CompareDistanceToView);
}


void ExperimentScene::ShadowPass(Shader& depth_shader)
{
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


	//instead of recursive render entities with children for shadowing use sorted opaque & transparent list
	//now using m_SceneEntitiiesWcRenderableMesh with already laid out mesh (_inc.ing children mesh)
	for (const auto& e : m_SceneEntitiesWcRenderableMesh)
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
	depth_shader.UnBind();
	RenderCommand::CullBack();
	RenderCommand::Viewport(0, 0, window->GetWidth(), window->GetHeight());
}

/// <summary>
/// Post GPU data is not require yet but whynot 
/// </summary>
void ExperimentScene::PostUpdateGPUUniformBuffers()
{
	long long int offset_pointer = 0;
	offset_pointer = 0;
	dirLightObject.dirlight.UpdateUniformBufferData(m_LightDataUBO, offset_pointer);
	//for (int i = 0; i < MAX_POINT_LIGHT; i++)
		//ptLight[i].UpdateUniformBufferData(m_LightDataUBO, offset_pointer);


	m_SceneShader.Bind();
	m_SceneShader.SetUniformVec3("u_ViewPos", m_Camera->GetPosition());

	m_SceneShader.SetUniform1i("u_EnableSceneShadow", enableSceneShadow);
	if (enableSceneShadow)
	{
		m_SceneShader.SetUniformMat4f("u_DirLightSpaceMatrix", dirLightObject.dirLightShadow.GetLightSpaceMatrix());
		//tex unit 0 >> texture 
		//tex unit 1 >> potenially normal map
		//tex unit 2 >> potenially parallax map
		//tex unit 3 >> shadow map (dir Light)
		//tex unit 4 >> shadow cube (pt Light)
		dirDepthMap.Read(3);
		m_SceneShader.SetUniform1i("u_DirShadowMap", 3);
		//ptDepthCube.Read(4);
		m_SceneShader.SetUniform1i("u_PointShadowCube", 4);
	}


	//Hack that there is no pt light the scene
	m_SceneShader.SetUniform1i("u_PtLightCount", 0);
	//environment
	offset_pointer = 0;
	m_EnviUBO.SetSubDataByID(&m_UseSkybox, sizeof(int), offset_pointer);
	offset_pointer += sizeof(int);
	m_EnviUBO.SetSubDataByID(&m_SkyboxInfluencity, sizeof(float), offset_pointer);
	offset_pointer += sizeof(float);
	m_EnviUBO.SetSubDataByID(&m_SkyboxReflectivity, sizeof(float), offset_pointer);
	//m_SceneShader.SetUniform1i("u_SkyboxMap", 5);
}

void ExperimentScene::DrawScene(Shader& main_shader)
{
	//Draw built scene 
	//what it might look like 
	//m_SceneShader.Bind();
	//std::vector<std::shared_ptr<Entity>> sorted;
	//auto mat = sorted[0]->GetMaterial();
	//MaterialShaderBindHelper(*mat, m_SceneShader);
	//for (auto& e : sorted)
	//{
	//	//check if diff
	//	if (mat->id != e->GetMaterial()->id)
	//	{
	//		mat = e->GetMaterial();
	//		MaterialShaderBindHelper(*mat, m_SceneShader);
	//	}

	//	//draw mesh
	//	//Renderer::DrawMesh(Mesh)
	//	//or Renderer::DrawMesh(Mesh, Transform)
	//	m_SceneShader.SetUniformMat4f("u_Model", e->GetWorldTransform());
	//	//material's not needed
	//	e->GetMesh()->Render(); //Later move render out of Mesh >> Renderer
	//}


	//for now 
	//this bind & unbind shader & material data every frame
	//for (auto& e : m_SceneEntities)
	//	e->Draw(m_SceneShader);


	//Move this up here has to render transparent object last 
	//Render Sky box
	//m_Skybox.Draw(*m_Camera, *window);

	//check if i could a normal shader to render skybox
	//but that would be useless as main model shader program as light etc 
	//things skybox does not require. 
	m_Skybox.Draw(m_SkyboxShader, m_SceneRenderer);
	

	main_shader.Bind();
	/////////////////////////////////////
	// DRAW OPAQUE ENTITIES FIRST
	///////////////////////////////////
	for (const auto& e : m_OpaqueEntites)
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
					MaterialShaderBindHelper(*m_SceneMaterials[0], main_shader);


				main_shader.SetUniformMat4f("u_Model", e.lock()->GetWorldTransform());
				m_SceneRenderer.DrawMesh(mesh);
			}

			//Dont need recursive childrens are already lay flat in the list during soring 
		}
	}
	

	/////////////////////////////////////
	// DRAW TRANSPARENT ENTITIES NEXT
	/////////////////////////////////////
	
	if (m_TransparentEntites.size() > 0)
	{
		RenderCommand::EnableBlend();
		RenderCommand::EnableDepthTest();
		RenderCommand::BlendFactor(BlendFactors::SRC_ALPHA, BlendFactors::ONE_MINUS_SCR_A);

		for (const auto& e : m_TransparentEntites)
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
						MaterialShaderBindHelper(*m_SceneMaterials[0], main_shader);


					main_shader.SetUniformMat4f("u_Model", e.lock()->GetWorldTransform());
					m_SceneRenderer.DrawMesh(mesh);
				}

				//Dont need recursive childrens are already lay flat in the list during soring 
			}
		}

		RenderCommand::DisableBlend();
	}

}

void ExperimentScene::PostProcess()
{
	RenderCommand::Clear();
	//render image to screen 
	//need a shader to read image
	m_PostImgShader.Bind();
	m_PostImgShader.SetUniform1f("exposure", hdrExposure);
	m_PostImgShader.SetUniform1f("u_Gamma", gamma);
	m_PostImgShader.SetUniform1i("u_DoHDR", m_DoHDR);
	m_SceneScreenFBO.BindTexture();
	m_SceneRenderer.DrawMesh(m_QuadMesh);
}

void ExperimentScene::SceneDebugger()
{
	return;
	const auto& cam = m_Camera;
	Frustum frustum = Frustum(cam->GetPosition(), cam->GetForward(),cam->GetUp(), *cam->Ptr_Near(), 
							 *cam->Ptr_Far(), *cam->Ptr_FOV(), window->GetAspectRatio());


	AABB a_aabb = AABB(glm::vec3(0.0f, 20.0f, 0.0f));
	a_aabb.Scale(glm::vec3(5.0f));
	glm::vec3 a_blue_col(0.0f, 0.0f, 1.0f);
	glm::vec3 a_red_col(1.0f, 0.0f, 0.0f);
	bool isInView = frustum.PointsInFrustum(a_aabb);
	glm::vec3 a_use_col = (isInView) ? a_blue_col : a_red_col;
	DebugGizmos::DrawBox(a_aabb, a_use_col);
	DebugGizmos::DrawSphere(a_aabb.GetCenter(), 0.2f, a_use_col);
	DebugGizmos::DrawFrustum(frustum, glm::vec3(1.0f, 0.0f, 0.0f));


	//test directional Shadow info 
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

	glm::vec3 translate, euler, scale;
	bool debug_scene_entities = true;
	if (debug_scene_entities)
	{
		TimeTaken("All Root AABB Enities Construction");
		for (const auto& e : m_SceneEntities)
		{
			std::string func_label = "Generating AABB for entity id: ";
			func_label += std::to_string(e->GetID());
			TimeTaken(func_label.c_str());


			AABB bounds;
			if (e->GetMesh())
			{
				//get entity mesh AABB
				bounds = e->GetMesh()->GetAABB();
				//transform aabb, based on enitity transformation
				bounds = bounds.Tranformed(e->GetWorldTransform());
				DebugGizmos::DrawBox(bounds);
			}
			//if e entity has children encapsulate their AABB
			if (e->GetChildren().size() > 0)
			{
				bounds = e->GetEncapsulatedChildrenAABB(); //already computed
				DebugGizmos::DrawBox(bounds, glm::vec3(0.0f, 0.0f, 1.0f), 2.0f);
			}
		}
	}

	bool debug_all_renderable_mesh_aabb = false;
	if (debug_all_renderable_mesh_aabb)
	{
		for (const auto& e : m_SceneEntitiesWcRenderableMesh)
		{
			if (!e.expired())
			{
				/////////////////////////////////////
				// ALRIGHT LETS GO 
				/////////////////////////////////////
				//get already cached AABB
				AABB bounds = e.lock()->GetMesh()->GetAABB();
				//aabb is located at the origin, lets transform based on entity world transform
				glm::mat4 e_trans = e.lock()->GetWorldTransform();
				bounds = bounds.Tranformed(e_trans);
				DebugGizmos::DrawBox(bounds, glm::vec3(1.0f, 0.0f, 0.0f));
			}
		}
	}




}

void ExperimentScene::DebugEntitiesPos(Entity& entity)
{
	DebugGizmos::DrawCross(entity.GetWorldTransform()[3], 2.5f);
	for (int i = 0; i < entity.GetChildren().size(); i++)
		DebugEntitiesPos(*entity.GetChildren()[i]);
}


void ExperimentScene::ResizeBuffers(unsigned int width, unsigned int height)
{
	m_PrevViewWidth = window->GetWidth();
	m_PrevViewHeight = window->GetHeight();
	m_SceneScreenFBO.ResizeBuffer(width, height);
	m_TopDownFBO.ResizeBuffer(width, height);
	m_TestGBuffer.ResizeBuffer(width, height);
}

/// <summary>
/// shader needs to be binded before passing
/// </summary>
/// <param name="mat"></param>
/// <param name="shader"></param>
void ExperimentScene::MaterialShaderBindHelper(Material& mat, Shader& shader)
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
	if (mat.parallaxMap)
	{
		mat.parallaxMap->Activate(tex_units);
		shader.SetUniform1i("u_Material.parallaxMap", tex_units++);
	}
	shader.SetUniform1i("u_Material.isTransparent", (mat.renderMode == CRRT_Mat::RenderingMode::Transparent));
	shader.SetUniform1i("u_Material.useNormal", mat.useNormal && mat.normalMap);
	shader.SetUniform1i("u_Material.shinness", mat.shinness);
	shader.SetUniform1i("u_Material.useParallax", mat.useParallax);
	shader.SetUniform1f("u_Material.parallax", mat.heightScale);
}

void ExperimentScene::MainUI()
{
	ImGui::Begin("Experimental ReWork Scene");

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

		float window_width = window->GetWidth();
		float window_height = window->GetHeight();
		static glm::mat4 test_proj;

		bool update_camera_proj = false;

		update_camera_proj = ImGui::SliderFloat("FOV", m_Camera->Ptr_FOV(), 0.0f, 179.0f, "%.1f");
		update_camera_proj += ImGui::DragFloat("Near", m_Camera->Ptr_Near(), 0.1f, 0.1f, 50.0f, "%.1f");
		update_camera_proj += ImGui::DragFloat("Far", m_Camera->Ptr_Far(), 0.1f, 0.0f, 500.0f, "%.1f");

		if (update_camera_proj)
		{
			glm::mat4 new_proj = m_Camera->CalculateProjMatrix(window->GetAspectRatio());
			//m_MainRenderer2.UpdateShaderViewProjection(new_proj);
		}


		ImGui::TreePop();
	}



	ImGui::Spacing();
	ImGui::SeparatorText("Scene Properties");
	ImGui::ColorEdit3("Debug colour", &m_ClearScreenColour[0]);
	ImGui::Text("Number of Opaque Entities: %d", m_OpaqueEntites.size());
	ImGui::Text("Number of Transparent Entities: %d", m_TransparentEntites.size());
	if (m_TransparentEntites.size() > 0)
	{
		ImGui::Text("Transparent Render Order:");
		for (const auto& e : m_TransparentEntites)
		{
			if (!e.expired())
			{
				ImGui::SameLine();
				ImGui::Text(" %d,", e.lock()->GetID());
			}
		}
	}
	ImGui::Spacing();
	if (ImGui::TreeNode("Post Scene Properties"))
	{
		ImGui::Checkbox("Do HDR", &m_DoHDR);
		ImGui::SliderFloat("Gamma", &gamma, 0.0f, 3.0f);
		ImGui::SliderFloat("HDR Exposure", &hdrExposure, 0.0f, 1.5f);
		ImGui::TreePop();
	}

	ImGui::Spacing();

	///////////////////////////////////////////
	// SCENE LIGHTS
	///////////////////////////////////////////
	ImGui::Spacing();
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::SeparatorText("Environment");
		ImGui::Checkbox("Use Skybox", &m_UseSkybox);
		ImGui::SliderFloat("Skybox influencity", &m_SkyboxInfluencity, 0.0f, 1.0f);
		ImGui::SliderFloat("Skybox reflectivity", &m_SkyboxReflectivity, 0.0f, 1.0f);

		ImGui::Spacing();
		ImGui::SeparatorText("Light Global Properties");
		ImGui::Checkbox("Enable Scene Shadow", &enableSceneShadow);



		//////////////////////////////////////
		// Directional Light
		//////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Directional Light");
		ImGui::Checkbox("Enable Directional", &dirLightObject.dirlight.enable);
		//ImGui::SameLine();
		//ImGui::Checkbox("Cast Shadow", &dirLightObject.dirlight.castShadow);
		ImGui::DragFloat3("Light Direction", &dirLightObject.dirlight.direction[0], 0.1f, -1.0f, 1.0f);
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



		ImGui::TreePop();
	}
	ImGui::End();
}

void ExperimentScene::EnititiesUI()
{
	if (m_SceneEntities.size() > 0)
	{
		ImGui::Begin("Entities Debug UI");
		for (auto& e : m_SceneEntities)
			EntityDebugUI(*e);
		ImGui::End();
	}
}

void ExperimentScene::EntityDebugUI(Entity& entity)
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
	if(entity.GetMaterial())
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


void ExperimentScene::EntityModelMaterial(const Entity& entity)
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

	for (int i = 0; i < entity.GetChildren().size(); i++)
	{
		EntityModelMaterial(*entity.GetChildren()[i]);
	}
}

void ExperimentScene::MaterialsUI()
{
	bool mat_ui = ImGui::Begin("Material");
	if (mat_ui)
	{
		static int blank_tex_id = blank_tex->GetID();

		int tex_id;
		//auto temp_mat = m_SceneMaterials[0];
		for (auto& mat : m_SceneMaterials)
		{
			//temp_mat
			ImGui::PushID(mat->name);
			ImGui::Text("1. %s", mat->name);
			int curr_sel = (int)mat->renderMode;
			if(ImGui::Combo("Rendering Mode", &curr_sel, CRRT_Mat::GetAllRenderingModesAsName(), (int)CRRT_Mat::RenderingMode::Count))
				mat->renderMode = (CRRT_Mat::RenderingMode)curr_sel;

			ImGui::ColorEdit4("Colour", &mat->baseColour[0]);
			tex_id = (mat->baseMap) ? mat->baseMap->GetID() : blank_tex_id;
			ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
			ImGui::SameLine(); ImGui::Text("Main Texture");
			ImGui::Checkbox("Use Normal", &mat->useNormal);
			tex_id = (mat->normalMap) ? mat->normalMap->GetID() : blank_tex_id;
			ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
			ImGui::SameLine(); ImGui::Text("Normal Map");
			tex_id = (mat->parallaxMap) ? mat->parallaxMap->GetID() : blank_tex_id;
			ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
			ImGui::SameLine(); ImGui::Text("Parallax/Height Map");
			ImGui::Checkbox("Use Parallax", &mat->useParallax);
			ImGui::SliderFloat("Parallax/Height Scale", &mat->heightScale, 0.0f, 0.08f);
			ImGui::SliderInt("Shinness", &mat->shinness, 32, 256);
			ImGui::PopID();
			//floorMat = temp_mat;
		}



		ImGui::Spacing();
		ImGui::SeparatorText("Debuggig Model Material");

		for(auto& e : m_SceneEntities)
			EntityModelMaterial(*e);
	}


	ImGui::End();
}

void ExperimentScene::EditTopViewUI()
{
	ImGui::Begin("Top Down View");

	ImGui::SeparatorText("Camera info");

	glm::vec3 temp = m_TopDownCamera.GetPosition();
	if (ImGui::DragFloat3("Cam pos", &temp[0], 0.1f))
		m_TopDownCamera.SetPosition(temp);

	ImGui::DragFloat("Cam Yaw", m_TopDownCamera.New_Yaw());
	ImGui::DragFloat("Cam Pitch", m_TopDownCamera.New_Pitch());


	if (ImGui::TreeNode("Extra Camera Properties"))
	{
		ImGui::SliderFloat("Cam FOV", m_TopDownCamera.Ptr_FOV(), 0.0f, 179.0f, "%.1f");
		ImGui::DragFloat("Cam Near", m_TopDownCamera.Ptr_Near(), 0.1f, 0.1f, 50.0f, "%.1f");
		ImGui::DragFloat("Cam Far", m_TopDownCamera.Ptr_Far(), 0.1f, 0.0f, 500.0f, "%.1f");

		ImGui::TreePop();
	}



	ImGui::SeparatorText("Frame Buffers");
	static int scale = 3;
	ImGui::SliderInt("Scale", &scale, 1, 5);
	ImVec2 img_size(500.0f * scale, 500.0f * scale);
	img_size.y *= (m_TopDownFBO.GetSize().y / m_TopDownFBO.GetSize().x); //invert
	ImGui::Image((ImTextureID)(intptr_t)m_TopDownFBO.GetColourAttachment(), img_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));


	ImGui::End();
}

void ExperimentScene::TestMRT_GBufferUI()
{
	ImGui::Begin("Experimenting GBuffer");
	static int scale = 3;
	ImGui::SliderInt("image scale", &scale, 1, 5);
	ImVec2 img_size(500.0f * scale, 500.0f * scale);
	img_size.y *= (m_TestGBuffer.GetSize().y / m_TestGBuffer.GetSize().x); //invert
	ImGui::Text("Colour Attachment Count: %d", m_TestGBuffer.GetColourAttachmentCount());
	//render image base on how many avaliable render tragets
	for (unsigned int i = 0; i < m_TestGBuffer.GetColourAttachmentCount(); i++)
	{
		ImGui::PushID(&i);//use this id 
		ImGui::Separator();
		ImGui::Image((ImTextureID)(intptr_t)m_TestGBuffer.GetColourAttachment(i), img_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::PopID();
	}

	ImGui::End();
}
