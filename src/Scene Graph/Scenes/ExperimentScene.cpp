#include "ExperimentScene.h"

#include "Renderer/RenderCommand.h"

#include "Renderer/Material.h"
#include "Util/FilePaths.h"

#include "Renderer/Meshes/Meshes.h"
#include "Scene Graph/Entity.h"

#include "Renderer/DebugGizmos.h"

#include "External Libs/imgui/imgui.h"
#include "Util/MathsHelpers.h"
#include <glm/gtx/quaternion.hpp>

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
	PreUpdateGPUUniformBuffers();
	BuildSceneEntities(); //<------Not implemented yet
	ShadowPass(shadowDepthShader); //<----- important for storing shadow data in texture 2D & texture cube map probab;y move to renderer and data is sent back 

	//Start Rendering
	RenderCommand::Clear();
	//post is also used for all shader view & shadow data but move to appropiate ubo later
	PostUpdateGPUUniformBuffers(); //<------- Not really necessary yet
	DrawScene();

	//Post Rendering (post Process) 


	//After Rendering (Clean-up/Miscellenous)
	SceneDebugger();
}

void ExperimentScene::OnRenderUI()
{
	MainUI();
	EnititiesUI();
	MaterialsUI();
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

	//////////////////////////////////////
	// GENERATE SHADERS
	//////////////////////////////////////
	//model shader
	ShaderFilePath shader_file_path
	{
		"Assets/Shaders/Learning/ParallaxExperiment/ParallaxModelVertex.glsl", //vertex shader
		"Assets/Shaders/Learning/ParallaxExperiment/ParallaxModelFrag.glsl", //fragment shader
	};
	m_SceneShader.Create("model_shader", shader_file_path);
	ShaderFilePath point_shadow_shader_file_path
	{
		"Assets/Shaders/ShadowMapping/ShadowDepthVertex.glsl", //vertex shader
		"Assets/Shaders/ShadowMapping/ShadowDepthFrag.glsl", //fragment shader
		"Assets/Shaders/ShadowMapping/ShadowDepthGeometry.glsl", //geometry shader
	};

	shadowDepthShader.Create("point_shadow_depth", point_shadow_shader_file_path);

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

	m_SceneMaterials.emplace_back(floorMat);
	m_SceneMaterials.emplace_back(plainMat);


	////////////////////////////////////////
	// CREATE ENTITIES 
	////////////////////////////////////////
	glm::mat4 temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
						   glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
						   glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));

	SquareMesh square_mesh;
	square_mesh.Create();
	CubeMesh cube_mesh;
	cube_mesh.Create();
	int id_idx = 0;
	Entity floor_plane_entity = Entity(id_idx++, "floor-plane-entity", temp_trans, std::make_shared<Mesh>(square_mesh), floorMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(floor_plane_entity));
	//move up 
	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.6f, 0.0f));
	Entity cube_entity = Entity(id_idx++, "cube-entity", temp_trans, std::make_shared<Mesh>(cube_mesh), plainMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(cube_entity));
	//move right
	temp_trans = glm::translate(temp_trans, glm::vec3(5.0f, 0.0f, 0.0f)) * 
				 glm::scale(temp_trans, glm::vec3(2.0f));
	Entity cube1_entity = Entity(id_idx++, "cube1-entity", temp_trans, std::make_shared<Mesh>(cube_mesh), plainMat);
	m_SceneEntities.emplace_back(std::make_shared<Entity>(cube1_entity));
	//move up & add to previous as world child & scale down
	temp_trans = glm::translate(temp_trans, glm::vec3(0.0f, 2.0f, 0.0f)) *
				 glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	Entity cube1child_entity = Entity(id_idx++, "cube1child-entity", temp_trans, std::make_shared<Mesh>(cube_mesh), plainMat);
	m_SceneEntities.back()->AddWorldChild(cube1child_entity);

	//create  a sphere with world pos an center
	//then add as cube1child_entity child
	SphereMesh sphere_mesh;
	sphere_mesh.Create();
	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.5f, 0.0f));// *
				// glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	Entity sphere_entity = Entity(id_idx++, "sphere-entity", temp_trans, std::make_shared<Mesh>(sphere_mesh), plainMat);
	//cube1child_entity.AddWorldChild(sphere_entity);
	//Quick Hack
	m_SceneEntities.back()->GetChildren()[0]->AddWorldChild(sphere_entity);
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


	//Assign UBO, if necessary 
	m_SceneShader.Bind();
	m_SceneShader.SetUniformBlockIdx("u_CameraMat", 0);
	m_SceneShader.SetUniformBlockIdx("u_LightBuffer", 1);


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

void ExperimentScene::PreUpdateGPUUniformBuffers()
{
	//------------------Camera Matrix Data UBO-----------------------------/
	m_CamMatUBO.SetSubDataByID(&(m_Camera->CalculateProjMatrix(window->GetAspectRatio())[0][0]), sizeof(glm::mat4), 0);
	m_CamMatUBO.SetSubDataByID(&(m_Camera->CalViewMat()[0][0]), sizeof(glm::mat4), sizeof(glm::mat4));

}


/// <summary>
/// Use this to build scene based on visibility 
/// etc
/// </summary>
void ExperimentScene::BuildSceneEntities()
{
	//
	// entity sorting / scene building 
	//	by material
	//	by mesh data
	//  by distance 
	//	by transparency / opacity
	//  by frustrum occulsion 
	//	by shadowcast
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
	shadowDepthShader.SetUniform1i("u_IsOmnidir", 0);
	shadowDepthShader.SetUniformMat4f("u_LightSpaceMat", dirLightObject.dirLightShadow.GetLightSpaceMatrix());
	//Draw Objects with material 
	//Renderer::DrawMesh(Mesh)


	for (auto& e : m_SceneEntities)
	{
		depth_shader.SetUniformMat4f("u_Model", e->GetWorldTransform());
		//material's not needed
		e->GetMesh()->Render(); //Later move render out of Mesh >> Renderer in use (as its should just be a data container)
		//check/traverse children
		//QuickHack 
		RenderEnitiyMesh(depth_shader, e);
	}
	dirDepthMap.UnBind(); 

	//point light shadows
	depth_shader.UnBind();
	RenderCommand::CullBack();
	RenderCommand::Viewport(0, 0, window->GetWidth(), window->GetHeight());
}

void ExperimentScene::RenderEnitiyMesh(Shader& shader, const std::shared_ptr<Entity>& entity)
{
	auto& children = entity->GetChildren();
	for (int i = 0; i < children.size(); i++)
	{
		//Render self
		shader.SetUniformMat4f("u_Model", children[i]->GetWorldTransform());
		children[i]->GetMesh()->Render();
		//Recursive transverse child
		RenderEnitiyMesh(shader, children[i]);
	}
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
}

void ExperimentScene::DrawScene()
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
	for (auto& e : m_SceneEntities)
		e->Draw(m_SceneShader);


	//Render Sky box
	m_Skybox.Draw(*m_Camera, *window);
}

void ExperimentScene::SceneDebugger()
{
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
}


/// <summary>
/// shader needs to be binded before passing
/// </summary>
/// <param name="mat"></param>
/// <param name="shader"></param>
void ExperimentScene::MaterialShaderBindHelper(Material& mat, Shader& shader)
{
	unsigned int tex_units = 0;
	shader.SetUniformVec3("u_Material.baseColour", mat.baseColour);
	if (mat.baseMap)
	{
		mat.baseMap->Activate(tex_units);
		shader.SetUniform1i("u_Material.baseMap", tex_units++);
	}
	bool has_nor = (mat.normalMap) ? true : false;
	if (has_nor)
	{
		mat.normalMap->Activate(tex_units);
		shader.SetUniform1i("u_Material.normalMap", tex_units++);
	}
	if (mat.parallaxMap)
	{
		mat.parallaxMap->Activate(tex_units);
		shader.SetUniform1i("u_Material.parallaxMap", tex_units++);
	}
	shader.SetUniform1i("u_UseNorMap", has_nor);
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
	ImGui::Spacing();

	///////////////////////////////////////////
	// SCENE LIGHTS
	///////////////////////////////////////////
	ImGui::Spacing();
	if (ImGui::TreeNode("Lights"))
	{
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
	ImGui::Text("Name: %s, ID: %d", entity.GetName(), entity.GetID());
	ImGui::Text("Number of Children: %d", entity.GetChildren().size());
	ImGui::SeparatorText("Transform");
	glm::vec3 translate, euler, scale;
	MathsHelper::DecomposeTransform(entity.GetTransform(), translate, euler, scale);
	bool update = ImGui::DragFloat3("Translate", &translate[0], 0.2f);
	update |= ImGui::DragFloat3("Rotation", &euler[0], 0.2f);
	update |= ImGui::DragFloat3("Scale", &scale[0], 0.2f);
	if (update)
	{
		entity.SetTransform(glm::translate(glm::mat4(1.0f), translate) *
			glm::toMat4(glm::quat(glm::radians(euler))) *
			glm::scale(glm::mat4(1.0f), scale));
	}
	ImGui::Text("Material Name: %s", entity.GetMaterial()->name);



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
			ImGui::ColorEdit3("Colour", &mat->baseColour[0]);
			tex_id = (mat->baseMap) ? mat->baseMap->GetID() : blank_tex_id;
			ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
			ImGui::SameLine(); ImGui::Text("Main Texture");
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
	}
	ImGui::End();
}
