#include "LightingModelingScene.h"

#include "Renderer/RendererErrorAssertion.h"
#include "Renderer/RenderCommand.h"

#include "Renderer/Meshes/PrimitiveMeshFactory.h"
#include "Renderer/Meshes/Mesh.h"
#include "Util/ModelLoader.h"

#include "Renderer/Material.h"

#include "libs/imgui/imgui.h"
#include "glm/gtc/quaternion.hpp"

#include "Util/MathsHelpers.h"
#include "Renderer/DebugGizmos.h"

void LightingModelingScene::SetWindow(Window* window)
{
	this->window = window;
}

void LightingModelingScene::OnInit(Window* window)
{
	Scene::OnInit(window);

	window->UpdateProgramTitle("Lighting Scene");

	RenderCommand::EnableDepthTest();
	RenderCommand::EnableFaceCull();

	RenderCommand::CullBack();

	//for skybox
	RenderCommand::DepthTestMode(DepthMode::LEEQUAL);

	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, 5.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 15.0f);

	CreateObjects();
}

void LightingModelingScene::OnUpdate(float delta_time)
{
	m_ShaderHotReload.Update();
	//Note(Jay): Probably need to fix this, Its best not having to manually Render in Update
	OnRender();
}

void LightingModelingScene::OnRender()
{
	RenderCommand::ClearColour(m_ClearScreenColour);
	RenderCommand::Clear();

	//update camera uniform buffer
	UpdateCameraUBO(*m_Camera, window->GetAspectRatio());


	//Draw Skybox 
	if(b_EnableSkybox)
		m_Skybox.Draw(m_SkyboxShader, m_SceneRenderer);


	//Update Shader datas like (lightings, skybox, etc)
	UpdateShaders();

	//plane 
	if (m_QuadMaterial)
	{
		MaterialShaderHelper(m_Shader, *m_QuadMaterial);
		//Draw planes
		for (const auto& model : m_PlaneTransform)
		{
			m_Shader.SetUniformMat4f("u_Model", model);
			m_SceneRenderer.DrawMesh(m_QuadMesh);
		}
	}


	//sphere
	if (m_SphereMaterial)
	{
		MaterialShaderHelper(m_Shader, *m_SphereMaterial);
		//Draw sphere
		for (const auto& model : m_SphereTransform)
		{
			m_Shader.SetUniformMat4f("u_Model", model);
			m_SceneRenderer.DrawMesh(m_SphereMesh);
		}
	}



	//cube
	if (m_CubeMaterial)
	{
		MaterialShaderHelper(m_Shader, *m_CubeMaterial);
		//Draw sphere
		for (const auto& model : m_CubeTransform)
		{
			m_Shader.SetUniformMat4f("u_Model", model);
			m_SceneRenderer.DrawMesh(m_CubeMesh);
		}
	}


	if (m_ConeMesh)
	{
		if (!m_CubeMaterial)
			MaterialShaderHelper(m_Shader, *m_CubeMaterial);
		//Draw sphere
		for (const auto& model : m_ConeTransform)
		{
			m_Shader.SetUniformMat4f("u_Model", model);
			m_SceneRenderer.DrawMesh(m_ConeMesh);
		}
	}


	if (b_DebugScene)
		DebugScene();
}

void LightingModelingScene::OnRenderUI()
{
	MainSceneUIEditor();
	MaterialEditorUI();
	SceneObjectTransformEditorUI();
}

void LightingModelingScene::OnDestroy()
{

	Scene::OnDestroy();
}

void LightingModelingScene::CreateObjects()
{
	auto& primitive_mesh_factory = CRRT::PrimitiveMeshFactory::Instance();
	m_SphereMesh = primitive_mesh_factory.CreateSphere();
	m_QuadMesh = primitive_mesh_factory.CreateQuad();
	m_CubeMesh = primitive_mesh_factory.CreateCube();
	auto& model_loader = CRRT::ModelLoader();
	m_ConeMesh = model_loader.LoadAsMesh("Assets/Models/blendershapes/blender_cone.fbx", true);


	ShaderFilePath shader_file_path
	{
		"Assets/Shaders/LightingScene/NewModelVertex.glsl", //vertex shader
		"Assets/Shaders/LightingScene/NewModelFragment.glsl", //fragment shader
	};
	m_Shader.Create("mesh_shader", shader_file_path);
	m_ShaderHotReload.TrackShader(&m_Shader);

	m_QuadMaterial = std::make_shared<BaseMaterial>();
	m_QuadMaterial->name = "Quad_Mat";
	m_SphereMaterial = std::make_shared<BaseMaterial>();
	m_SphereMaterial->name = "Sphere_Mat";
	m_CubeMaterial = std::make_shared<BaseMaterial>();
	m_CubeMaterial->name = "Cube_Mat";

	m_SphereMaterial->ambient = glm::vec3(0.3710993f, 0.07421f, 0.0468f);
	m_SphereMaterial->diffuse = glm::vec3(0.96875f, 0.f, 0.1406f);
	m_SphereMaterial->specular = glm::vec3(0.67578f, 0.37109f, 0.35156f);
	m_PreviewMaterial = m_SphereMaterial;

	m_CubeMaterial->ambient = glm::vec3(0.109375f, 0.35546f, 0.578125f);
	m_CubeMaterial->diffuse = glm::vec3(0.1406f, 0.f, 0.96875f);
	m_CubeMaterial->specular = glm::vec3(0.4726f, 0.8984f, 0.9921f);



	////////////////////////////////////////
	// UNIFORM BUFFERs
	////////////////////////////////////////
	//------------------Camera Matrix Data UBO-----------------------------/
	long long int buf_size = sizeof(glm::vec3);//for view pos
	buf_size += sizeof(float);// camera far
	buf_size += 2 * sizeof(glm::mat4);// +sizeof(glm::vec2);   //to store view, projection
	m_CameraUBO.Generate(buf_size);
	m_CameraUBO.BindBufferRndIdx(0, buf_size, 0);

	////////////////////////////////////////
	// Directional Light
	////////////////////////////////////////
	m_DirLight.direction = glm::vec3(-1.0f, 1.0f, -0.2f);

	////////////////////////////////////////
	// Point Light
	////////////////////////////////////////
	m_PointLight.position = glm::vec3(7.5f, 3.0f, 0.0f);
	m_PointLight.enable = true;

	////////////////////////////////////////
	// Spot Light
	////////////////////////////////////////
	m_SpotLight.diffuse[0] = 1.0f;
	m_SpotLight.diffuse[1] = 1.0f;
	m_SpotLight.diffuse[2] = 1.0f;
	m_SpotLight.direction = glm::vec3(-1.0f, 1.0f, -0.2f);
	m_SpotLight.position = glm::vec3(-12.0f, 3.0f, 0.0f);
	m_SpotLight.enable = true;

	//Scene object transformations
	glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)) *
					glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
					glm::scale(glm::mat4(1.0f), glm::vec3(50.0f));
	m_PlaneTransform.push_back(mat);

	//Spheres
	mat = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, 1.0f, 0.0f));
	int count_per_axis = 2;
	glm::vec2 offset(1.5f);
	for (int i = 0; i < count_per_axis; i++)
	{
		float x = (float)i * offset.x;
		for (int j = 0; j < count_per_axis; j++)
		{
			m_SphereTransform.push_back(glm::translate(mat, glm::vec3(x, 0.0f, (float)j * offset.y)));
		}
	}

	//Cubes
	mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	for (int i = 0; i < count_per_axis; i++)
	{
		float x = (float)i * offset.x;
		for (int j = 0; j < count_per_axis; j++)
		{
			m_CubeTransform.push_back(glm::translate(mat, glm::vec3(x, 0.0f, (float)j * offset.y)));
		}
	}

	//Cones
	mat = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	offset = glm::vec2(2.0f);
	for (int i = 0; i < count_per_axis; i++)
	{
		float x = (float)i * offset.x;
		for (int j = 0; j < count_per_axis; j++)
		{
			m_ConeTransform.push_back(glm::translate(mat, glm::vec3(x, (float)j * -offset.y, 0.0f)));
		}
	}

	//Material Preview 
	m_MaterialPreviewCamera =  Camera(glm::vec3(0.0f, 0.0f, -3.5f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 5.0f);
	*m_MaterialPreviewCamera.Ptr_Far() = 5.0f;
	*m_MaterialPreviewCamera.Ptr_FOV() = 30.0f;
	m_MaterialPreviewFBO.Generate(512, 512);



	//Skybox
	std::vector<std::string> skybox_faces
	{
		"Assets/Textures/Skyboxes/default_skybox/right.jpg",
		"Assets/Textures/Skyboxes/default_skybox/left.jpg",
		"Assets/Textures/Skyboxes/default_skybox/top.jpg",
		"Assets/Textures/Skyboxes/default_skybox/bottom.jpg",
		"Assets/Textures/Skyboxes/default_skybox/front.jpg",
		"Assets/Textures/Skyboxes/default_skybox/back.jpg",
	};
	m_Skybox.Create(skybox_faces);

	ShaderFilePath skybox_shader_file_path
	{
		"Assets/Shaders/Utilities/Skybox/SkyboxVertex.glsl", //vertex shader
		"Assets/Shaders/Utilities/Skybox/SkyboxFragment.glsl", //fragment shader
	};
	m_SkyboxShader.Create("skybox_shader", skybox_shader_file_path);
}

void LightingModelingScene::UpdateCameraUBO(Camera& cam, float aspect_ratio)
{
	//------------------Camera Matrix Data UBO-----------------------------/
	unsigned int offset_ptr = 0;
	m_CameraUBO.SetSubDataByID(&(cam.GetPosition()[0]), sizeof(glm::vec3), offset_ptr);
	offset_ptr += sizeof(glm::vec3);
	m_CameraUBO.SetSubDataByID(cam.Ptr_Far(), sizeof(float), offset_ptr);
	offset_ptr += sizeof(float);
	m_CameraUBO.SetSubDataByID(&(cam.CalculateProjMatrix(aspect_ratio)[0][0]), sizeof(glm::mat4), offset_ptr);
	offset_ptr += sizeof(glm::mat4);
	m_CameraUBO.SetSubDataByID(&(cam.CalViewMat()[0][0]), sizeof(glm::mat4), offset_ptr);
}

void LightingModelingScene::UpdateShaders()
{
	m_Shader.Bind();


	//------------Skybox-----------------------
	m_Shader.SetUniform1i("u_UseSkybox", b_EnableSkybox);
	m_Skybox.ActivateMap();

	
	//------Render type Blinn-Phong/Phong---------
	m_Shader.SetUniform1i("u_PhongRendering", b_PhongRendering);


	//---------directional Light-------------------
	//vec3s
	//direction 
	m_Shader.SetUniformVec3("u_DirectionalLight.direction", m_DirLight.direction);
	//diffuse
	m_Shader.SetUniformVec3f("u_DirectionalLight.diffuse", &m_DirLight.diffuse[0]);
	//ambient
	m_Shader.SetUniformVec3f("u_DirectionalLight.ambient", &m_DirLight.ambient[0]);
	//specular
	m_Shader.SetUniformVec3f("u_DirectionalLight.specular", &m_DirLight.specular[0]);
	m_Shader.SetUniform1i("u_DirectionalLight.enable", m_DirLight.enable);

	//---------point light-------------------
	//position;
	m_Shader.SetUniformVec3("u_PointLight.position", m_PointLight.position);
	//enable;
	m_Shader.SetUniform1i("u_PointLight.enable", m_PointLight.enable);
	//
	//diffuse;
	m_Shader.SetUniformVec3f("u_PointLight.diffuse", &m_PointLight.diffuse[0]);
	//vec3 ambient;
	m_Shader.SetUniformVec3f("u_PointLight.ambient", &m_PointLight.ambient[0]);
	//specular;
	m_Shader.SetUniformVec3f("u_PointLight.specular", &m_PointLight.specular[0]);
	//constantAtt;
	m_Shader.SetUniform1f("u_PointLight.constantAtt", m_PointLight.attenuation[0]);
	//linearAtt;
	m_Shader.SetUniform1f("u_PointLight.linearAtt", m_PointLight.attenuation[1]);
	//quadraticAtt;
	m_Shader.SetUniform1f("u_PointLight.quadraticAtt", m_PointLight.attenuation[2]);


	//---------spot light-------------------
	//position;
	m_Shader.SetUniformVec3("u_SpotLight.position", m_SpotLight.position);
	//enable;
	m_Shader.SetUniform1i("u_SpotLight.enable", m_SpotLight.enable);
	//
	//diffuse;
	m_Shader.SetUniformVec3f("u_SpotLight.diffuse", &m_SpotLight.diffuse[0]);
	//vec3 ambient;
	m_Shader.SetUniformVec3f("u_SpotLight.ambient", &m_SpotLight.ambient[0]);
	//specular;
	m_Shader.SetUniformVec3f("u_SpotLight.specular", &m_SpotLight.specular[0]);
	//constantAtt;
	m_Shader.SetUniform1f("u_SpotLight.constantAtt", m_SpotLight.attenuation[0]);
	//linearAtt;
	m_Shader.SetUniform1f("u_SpotLight.linearAtt", m_SpotLight.attenuation[1]);
	//quadraticAtt;
	m_Shader.SetUniform1f("u_SpotLight.quadraticAtt", m_SpotLight.attenuation[2]);
	//direction;
	m_Shader.SetUniformVec3("u_SpotLight.direction", m_SpotLight.direction);
	//innerCutoffAngle;
	m_Shader.SetUniform1f("u_SpotLight.innerCutoffAngle", glm::cos(glm::radians(m_SpotLight.innerCutoffAngle)));
	//m_Shader.SetUniform1f("u_SpotLight.innerCutoffAngle", glm::radians(m_SpotLight.innerCutoffAngle * 0.5f));
	//outerCutoffAngle;
	//m_Shader.SetUniform1f("u_SpotLight.outerCutoffAngle", glm::radians(m_SpotLight.outerCutoffAngle * 0.5f));
	m_Shader.SetUniform1f("u_SpotLight.outerCutoffAngle", glm::cos(glm::radians(m_SpotLight.outerCutoffAngle)));
	m_Shader.SetUniform1i("u_SpotLight.debug", m_SpotLight.debug);
}

void LightingModelingScene::MainSceneUIEditor()
{
	if (ImGui::Begin("Lighting Modeling Scene"))
	{
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

		ImGui::Text("Pitch: %f", *m_Camera->Ptr_Pitch());
		ImGui::Text("Yaw: %f", *m_Camera->Ptr_Yaw());


		if (ImGui::TreeNode("Camera Properties"))
		{
			ImGui::SliderFloat("Move Speed", m_Camera->Ptr_MoveSpeed(), 5.0f, 250.0f);
			ImGui::SliderFloat("Rot Speed", m_Camera->Ptr_RotSpeed(), 0.0f, 10.0f, "%.1f");

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



		Ext_EnvPropUIPanelSection();
		Ext_LightingUIPanelSection();


		ImGui::End();
	}
}

void LightingModelingScene::Ext_LightingUIPanelSection()
{
	//////////////////////////////////////
	// Directional Light
	//////////////////////////////////////
	ImGui::Spacing();
	ImGui::SeparatorText("Directional Light");
	ImGui::Checkbox("Enable", &m_DirLight.enable);
	ImGui::DragFloat3("Light Direction", &m_DirLight.direction[0], 0.1f, -1.0f, 1.0f);
	ImGui::ColorEdit3("Diffuse colour", &m_DirLight.diffuse[0]);
	ImGui::ColorEdit3("Ambient colour", &m_DirLight.ambient[0]);
	ImGui::ColorEdit3("Specular colour", &m_DirLight.specular[0]);

	//////////////////////////////////////
	// Point Light
	//////////////////////////////////////
	ImGui::Spacing();
	ImGui::SeparatorText("Point Light");
	ImGui::PushID(&m_PointLight);
	ImGui::Checkbox("Enable", &m_PointLight.enable);
	ImGui::DragFloat3("Light Position", &m_PointLight.position[0], 0.1f);
	ImGui::ColorEdit3("Diffuse colour", &m_PointLight.diffuse[0]);
	ImGui::ColorEdit3("Ambient colour", &m_PointLight.ambient[0]);
	ImGui::ColorEdit3("Specular colour", &m_PointLight.specular[0]);
	ImGui::SliderFloat("Constant attenuation", &m_PointLight.attenuation[0], 0.1f, 1.0f);
	ImGui::SliderFloat("Linear attenuation", &m_PointLight.attenuation[1], 0.0f, 0.1f);
	ImGui::SliderFloat("Quadratic attenuation", &m_PointLight.attenuation[2], 0.0f, 0.01f);
	ImGui::PopID();

	//////////////////////////////////////
	// Spot Light
	//////////////////////////////////////
	ImGui::Spacing();
	ImGui::SeparatorText("Spot Light");
	ImGui::PushID(&m_SpotLight);
	ImGui::Checkbox("Enable", &m_SpotLight.enable);
	ImGui::DragFloat3("Light Position", &m_SpotLight.position[0], 0.1f);
	ImGui::DragFloat3("Light Direction", &m_SpotLight.direction[0], 0.1f);
	ImGui::ColorEdit3("Diffuse colour", &m_SpotLight.diffuse[0]);
	ImGui::ColorEdit3("Ambient colour", &m_SpotLight.ambient[0]);
	ImGui::ColorEdit3("Specular colour", &m_SpotLight.specular[0]);
	ImGui::SliderFloat("Constant attenuation", &m_SpotLight.attenuation[0], 0.1f, 1.0f);
	ImGui::SliderFloat("Linear attenuation", &m_SpotLight.attenuation[1], 0.0f, 0.1f);
	ImGui::SliderFloat("Quadratic attenuation", &m_SpotLight.attenuation[2], 0.0f, 0.01f);
	ImGui::SliderFloat("Inner cut-off", &m_SpotLight.innerCutoffAngle, 0.1f, m_SpotLight.outerCutoffAngle, "%.1f");
	ImGui::SliderFloat("Outer cut-off", &m_SpotLight.outerCutoffAngle, m_SpotLight.innerCutoffAngle, 45.0f, "%.1f");
	ImGui::Checkbox("Debug", &m_SpotLight.debug);
	ImGui::PopID();
}

void LightingModelingScene::Ext_EnvPropUIPanelSection()
{
	ImGui::Spacing();
	ImGui::SeparatorText("Environment");
	ImGui::Checkbox("Enable Skybox", &b_EnableSkybox);
	ImGui::Checkbox("Debug scene", &b_DebugScene);
	ImGui::Checkbox("Phong Rendering", &b_PhongRendering);
}

void LightingModelingScene::SceneObjectTransformEditorUI()
{
	if (ImGui::Begin("TransformEditor"))
	{
		int id = 0;
		ImGui::SeparatorText("Plane");
		if (ImGui::TreeNode("Plane"))
		{
			for (auto& transform : m_PlaneTransform)
			{
				ImGui::Separator();
				ImGui::PushID(id++);
				Ext_TransformEditorPanel(transform);
				ImGui::PopID();
			}
			ImGui::TreePop();
		}


		if (ImGui::TreeNode("Spheres"))
		{
			for (auto& transform : m_SphereTransform)
			{
				ImGui::Separator();
				ImGui::PushID(id++);
				Ext_TransformEditorPanel(transform);
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		

		if (ImGui::TreeNode("Cubes"))
		{
			for (auto& transform : m_CubeTransform)
			{
				ImGui::Separator();
				ImGui::PushID(id++);
				Ext_TransformEditorPanel(transform);
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Cone"))
		{
			for (auto& transform : m_ConeTransform)
			{
				ImGui::Separator();
				ImGui::PushID(id++);
				Ext_TransformEditorPanel(transform);
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		ImGui::End();
	}
}

bool LightingModelingScene::Ext_TransformEditorPanel(glm::mat4& transform)
{
	bool update = false;

	glm::vec3 translate;
	glm::vec3 euler;
	glm::vec3 scale;
	MathsHelper::DecomposeTransform(transform, translate, euler, scale);
	update |= ImGui::DragFloat3("Position", &translate[0], 0.2f);
	update |= ImGui::DragFloat3("Euler", &euler[0], 0.2f);
	update |= ImGui::DragFloat3("Scale", &scale[0], 0.2f);
	
	if(update)
		transform = glm::translate(glm::mat4(1.0f), translate) * 
					glm::mat4_cast(glm::quat(glm::radians(euler))) * 
					glm::scale(glm::mat4(1.0f), scale);

	return false;
}

void LightingModelingScene::MaterialEditorUI()
{
	static bool update_preview_fbo = true;
	static int mesh_id = 0;
	static DirectionalLight preview_lighting = DirectionalLight(glm::vec3(-1.0f, 1.0f, -1.0f));

	if (update_preview_fbo)
	{
		m_MaterialPreviewFBO.Bind();
		RenderCommand::ClearColour(glm::vec3(0.2f));
		RenderCommand::Clear();
		//Update camera 
		float aspect_ratio = m_MaterialPreviewFBO.GetSize().y / m_MaterialPreviewFBO.GetSize().x;
		UpdateCameraUBO(m_MaterialPreviewCamera, aspect_ratio);
		//tranform matrix later
		m_Shader.Bind();
		//with the hope that other parameter are updated like 
		//the lighting in  
		m_Shader.SetUniformMat4f("u_Model", m_PreviewTransform);
		m_Shader.SetUniform1i("u_OnlyBaseColour", !b_MaterialPreviewLighting);
		m_Shader.SetUniform1i("u_PhongRendering", !m_PreviewMaterial.lock()->blinn_phong);
		//vec3s
		//direction 
		m_Shader.SetUniformVec3("u_DirectionalLight.direction", preview_lighting.direction);
		//diffuse
		m_Shader.SetUniformVec3f("u_DirectionalLight.diffuse", &preview_lighting.diffuse[0]);
		//ambient
		m_Shader.SetUniformVec3f("u_DirectionalLight.ambient", &preview_lighting.ambient[0]);
		//specular
		m_Shader.SetUniformVec3f("u_DirectionalLight.specular", &preview_lighting.specular[0]);
		MaterialShaderHelper(m_Shader, *m_PreviewMaterial.lock());

		auto preview_mesh = m_SphereMesh;
		if (mesh_id == 1)
			preview_mesh = m_QuadMesh;
		else if (mesh_id == 2)
			preview_mesh = m_CubeMesh;

		m_SceneRenderer.DrawMesh(preview_mesh);
		m_Shader.SetUniform1i("u_BasicPhong", false);

		m_MaterialPreviewFBO.UnBind();
		update_preview_fbo = false;
		RenderCommand::Viewport(0, 0, window->GetWidth(), window->GetHeight());
	}

	if (ImGui::Begin("Materials Editor"))
	{
		ImVec2 ui_win_size = ImGui::GetWindowSize();
		ImVec2 preview_panel_size = ImVec2(ui_win_size.x * 0.5f, ui_win_size.x * 0.5f);
		preview_panel_size.y *= (m_MaterialPreviewFBO.GetSize().y / m_MaterialPreviewFBO.GetSize().x); //invert

		ImVec2 top_left = ImGui::GetCursorPos();
		ImGui::Image((ImTextureID)(intptr_t)m_MaterialPreviewFBO.GetColourAttachment(), 
					 preview_panel_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		update_preview_fbo |= Ext_QuatEditorPanel(m_PreviewTransform, top_left, preview_panel_size);


		ImGui::SameLine();
		ImGui::BeginChild("##mesh_type", ImVec2(preview_panel_size.x * 0.75f, preview_panel_size.y * 0.5f));
		const char* mesh_type[] = { "Sphere", "Quad", "Cube" };
		update_preview_fbo |= ImGui::Combo("Preview Mesh", &mesh_id, mesh_type, 3);
		ImGui::EndChild();

		//material properties
		auto& mat = m_PreviewMaterial.lock();
		ImGui::Text(mat->name);
		update_preview_fbo |= ImGui::Checkbox("Preview Lighting or Base colour", &b_MaterialPreviewLighting);
		update_preview_fbo |= ImGui::Checkbox("Blinn-Phong", &mat->blinn_phong);
		update_preview_fbo |= ImGui::ColorEdit3("Diffuse", &mat->diffuse[0]);
		update_preview_fbo |= ImGui::ColorEdit3("Ambient", &mat->ambient[0]);
		update_preview_fbo |= ImGui::ColorEdit3("Specular", &mat->specular[0]);
		update_preview_fbo |= ImGui::SliderFloat("Shinness", &mat->shinness, 8.0f, 256.0f, "%.0f");
		int tex_id = (mat->normalMap) ? mat->normalMap->GetID() : blank_tex->GetID();
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));

		//list all materials
		if (ImGui::Button("Sphere Material"))
		{
			m_PreviewMaterial = m_SphereMaterial;
			update_preview_fbo |= true;
		}
		if (ImGui::Button("Quad Material"))
		{
			m_PreviewMaterial = m_QuadMaterial;
			update_preview_fbo |= true;
		}
		if (ImGui::Button("Cube Material"))
		{
			m_PreviewMaterial = m_CubeMaterial;
			update_preview_fbo |= true;
		}


		if (ImGui::TreeNode("Preview DirectionalLight"))
		{
			update_preview_fbo |= ImGui::Checkbox("Enable", &preview_lighting.enable);
			update_preview_fbo |= ImGui::DragFloat3("Light Direction", &preview_lighting.direction[0], 0.1f, -1.0f, 1.0f);
			update_preview_fbo |= ImGui::ColorEdit3("Diffuse colour", &preview_lighting.diffuse[0]);
			update_preview_fbo |= ImGui::ColorEdit3("Ambient colour", &preview_lighting.ambient[0]);
			update_preview_fbo |= ImGui::ColorEdit3("Specular colour", &preview_lighting.specular[0]);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Preview Camera"))
		{
			static bool update_cam = false;

			update_cam |= ImGui::SliderFloat("FOV", m_MaterialPreviewCamera.Ptr_FOV(), 0.0f, 179.0f, "%.1f");
			update_cam |= ImGui::DragFloat("Near", m_MaterialPreviewCamera.Ptr_Near(), 0.1f, 0.1f, 50.0f, "%.1f");
			update_cam |= ImGui::DragFloat("Far", m_MaterialPreviewCamera.Ptr_Far(), 0.1f, 0.0f, 500.0f, "%.1f");

			glm::vec3 pos = m_MaterialPreviewCamera.GetPosition();
			if (update_cam |= ImGui::DragFloat3("Position", &pos[0], 0.1f))
				m_MaterialPreviewCamera.SetPosition(pos);

			update_cam |= ImGui::SliderFloat("Yaw", m_MaterialPreviewCamera.Ptr_Yaw(), 0.0f, 360.0f);
			update_cam |= ImGui::SliderFloat("Pitch", m_MaterialPreviewCamera.Ptr_Pitch(), 0.0f, 360.0f);

			//quick hack
			if (update_cam)
				m_MaterialPreviewCamera.SetPosition(pos);

			update_preview_fbo |= update_cam;

			ImGui::TreePop();
		}

		ImGui::End();
	}
}

bool LightingModelingScene::Ext_QuatEditorPanel(glm::mat4& transform, ImVec2 top_left , ImVec2 size)
{
	glm::quat quat = glm::quat_cast(transform);


	ImVec2 ui_win_size = ImGui::GetWindowSize();
	//ImVec2 drag_panel_size = ImVec2(200, 200);
	ImVec2 drag_panel_size = ImVec2(ui_win_size.x * 0.5f, ui_win_size.x * 0.5f);
	static float drag_senstitivity = 0.001f;
	static int drag_speed = 5;
	static bool invert_x = false;
	static bool invert_y = false;
	
	
	//need location
	ImGui::SetCursorPos(top_left);
	ImGui::InvisibleButton("##quat_editor", size);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();
	draw_list->AddRect(min, max, IM_COL32(255, 255, 255, 255));


	//mouse drag
	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
	{
		draw_list->AddRect(min, max, IM_COL32(0, 0, 255, 255));
		ImVec2 dt_drag = ImGui::GetMouseDragDelta();

		//dt drag -> rot angle
		float dt_x = dt_drag.x * drag_senstitivity * drag_speed;// *(invert_x) ? -1.0f : 1.0f;
		float dt_y = dt_drag.y * drag_senstitivity * drag_speed;

		dt_x *= (invert_x) ? -1.0f : 1.0f;
		dt_y *= (invert_y) ? 1.0f : -1.0f;

		glm::quat rot_x = glm::angleAxis(dt_y, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat rot_y = glm::angleAxis(dt_x, glm::vec3(0.0f, 1.0f, 0.0f));

		quat = glm::normalize(rot_y * rot_x * quat);

		transform = glm::mat4_cast(quat);

		ImGui::ResetMouseDragDelta();
		return true;
	}

	return false;
}

void LightingModelingScene::MaterialShaderHelper(Shader& shader, const BaseMaterial& mat)
{
	//Material (u_Material)
	//diffuse; vec3s
	shader.SetUniformVec3("u_Material.diffuse", mat.diffuse);
	//ambient;
	shader.SetUniformVec3("u_Material.ambient", mat.ambient);
	//specular;
	shader.SetUniformVec3("u_Material.specular", mat.specular);
	//shinness; float
	shader.SetUniform1f("u_Material.shinness", mat.shinness);
}

void LightingModelingScene::DebugScene()
{


	//DebugGizmos::DrawWireThreeDisc(m_PointLight.position, m_PointLight.CalculateLightRadius(0.2f), 
	//							   15, glm::vec3(1.0f, 1.0f, 0.0f));


	//DebugGizmos::DrawWireCone(m_SpotLight.position + (15.0f * -m_SpotLight.direction), -m_SpotLight.direction, m_SpotLight.CalculateLightRadius(0.2f),
	//						  15.0f, glm::vec3(1.0f, 1.0f, 0.0f));

	DebugGizmos::DrawSpotLight(m_SpotLight.position, -m_SpotLight.direction, m_SpotLight.CalculateLightRadius(0.2f), 
							   m_SpotLight.innerCutoffAngle, m_SpotLight.outerCutoffAngle);

	//DebugGizmos::DrawSpotLight(d_SpotPos, d_SpotDir, d_SpotRange, d_SpotInner, d_SpotOuter, glm::vec3(1.0f, 1.0f, 0.0f), d_Segment);

	//draw all bactches 
	DebugGizmos::DrawAllBatches();
}


