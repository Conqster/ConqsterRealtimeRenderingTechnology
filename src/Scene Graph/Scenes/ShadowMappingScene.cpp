#include "ShadowMappingScene.h"

#include "Renderer/RenderCommand.h"

#include "Renderer/Meshes/PrimitiveMeshFactory.h"
#include "Renderer/Material.h"

#include "Renderer/DebugGizmos.h"

#include "libs/imgui/imgui.h"

#include "Util/MathsHelpers.h"
#include "glm/gtc/quaternion.hpp"

#include "Util/ModelLoader.h"

void ShadowMappingScene::SetWindow(Window* window)
{
	this->window = window;
}

void ShadowMappingScene::OnInit(Window* window)
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

void ShadowMappingScene::OnUpdate(float delta_time)
{
	//Animate Light Quick Hack
	m_PtLightAnimateProp.Update(m_PointLight.position, m_AnimateObjectInstanceAngle[0], delta_time);

	glm::vec3 rot_dir = glm::vec3(1.0f, 0.0f, 0.0f);
	for (auto& m : m_CubeTransform)
	{
		m = glm::rotate(m, glm::radians(3.f), rot_dir);
		rot_dir.x = ((int)rot_dir.x + 1) % 2;
		rot_dir.y = ((int)rot_dir.y + 1) % 2;
	}
	for (auto& m : m_ModelTransform)
		m = glm::rotate(m, glm::radians(1.f), glm::vec3(0.0f, 1.0f, 0.0f));

	for (int i = 0; i < 2; i++)
	{
		glm::vec3 t;
		glm::vec3 r;
		glm::vec3 s;
		MathsHelper::DecomposeTransform(m_SphereTransform[i], t, r, s);
		m_PtLightAnimateProp.Update(t, m_AnimateObjectInstanceAngle[i + 1], delta_time);
		m_SphereTransform[i] = glm::translate(glm::mat4(1.0f), t) *
			glm::mat4_cast(glm::quat(glm::radians(r))) *
			glm::scale(glm::mat4(1.0f), s);

		MathsHelper::DecomposeTransform(m_CubeTransform[i], t, r, s);

		m_PtLightAnimateProp.Update(t, m_AnimateObjectInstanceAngle[i + 3], delta_time);
		//m_PtLightAnimateProp.Update(r, m_AnimateObjectInstanceAngle[i + 3], 1.0f);
		m_CubeTransform[i] = glm::translate(glm::mat4(1.0f), t) *
							glm::mat4_cast(glm::quat(glm::radians(r))) * glm::scale(glm::mat4(1.0f), s);
		//m_CubeTransform[i] = euler;
	}


	//Note(Jay): Probably need to fix this, Its best not having to manually Render in Update
	OnRender();
	m_ShaderHotReload.Update();
}

void ShadowMappingScene::OnRender()
{
	RenderCommand::ClearColour(m_ClearScreenColour);
	RenderCommand::Clear();

	//update camera uniform buffer
	UpdateCameraUBO(*m_Camera, window->GetAspectRatio());

	//Draw Skybox 
	if (b_EnableSkybox)
		m_Skybox.Draw(m_SkyboxShader, m_SceneRenderer);

	ShadowPass();
	RenderCommand::Viewport(0, 0, window->GetWidth(), window->GetHeight());

	//Update Shader datas like (lightings, skybox, etc)
	UpdateShaders();
	
	//DrawObjects(DebugGizmos::GetDebugMeshShader(), true);
	DrawObjects(m_Shader);

	if (b_DebugScene)
		DebugScene();


	//reset stuffs
	b_FrameHasShadow = false;
}

void ShadowMappingScene::OnRenderUI()
{
	static bool update_dir_shadow;
	MainSceneUIEditor(update_dir_shadow);
	SceneObjectTransformEditorUI();
	ShadowPropEditorUI(update_dir_shadow);
	AnimatedObjectPropertiesEditorUI();

	if (update_dir_shadow)
	{
		m_DirLightShadowProp.UpdateProjMat();
		m_DirLightShadowProp.UpdateViewMatrix(m_DirLightShadowProp.sampleWorldPos,
			m_DirLight.direction, m_DirLightShadowProp.cam_offset);
	}
}

void ShadowMappingScene::OnDestroy()
{
	Scene::OnDestroy();
}

void ShadowMappingScene::CreateObjects()
{
	auto& primitive_mesh_factory = CRRT::PrimitiveMeshFactory::Instance();
	m_SphereMesh = primitive_mesh_factory.CreateSphere();
	m_QuadMesh = primitive_mesh_factory.CreateQuad();
	m_CubeMesh = primitive_mesh_factory.CreateCube();

	auto& model_loader = CRRT::ModelLoader();
	m_ModelMesh = std::make_shared<Mesh>(model_loader.LoadAsSingleMesh("Assets/Models/stanford-bunny.obj"));

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

	mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	m_ModelTransform.push_back(mat);

	//Init animated object angle
	//pt light 
	m_AnimateObjectInstanceAngle.push_back(0.0f);
	//muliple objects
	m_AnimateObjectInstanceAngle.push_back(glm::radians(30.0f));
	m_AnimateObjectInstanceAngle.push_back(glm::radians(100.0f));
	m_AnimateObjectInstanceAngle.push_back(glm::radians(130.0f));
	m_AnimateObjectInstanceAngle.push_back(glm::radians(230.0f));


	//scene materials
	m_QuadMaterial = std::make_shared<BaseMaterial>();
	m_QuadMaterial->name = "Quad_Mat";
	m_SphereMaterial = std::make_shared<BaseMaterial>();
	m_SphereMaterial->name = "Sphere_Mat";
	m_CubeMaterial = std::make_shared<BaseMaterial>();
	m_CubeMaterial->name = "Cube_Mat";

	m_SphereMaterial->ambient = glm::vec3(0.3710993f, 0.07421f, 0.0468f);
	m_SphereMaterial->diffuse = glm::vec3(0.96875f, 0.f, 0.1406f);
	m_SphereMaterial->specular = glm::vec3(0.67578f, 0.37109f, 0.35156f);

	m_CubeMaterial->ambient = glm::vec3(0.109375f, 0.35546f, 0.578125f);
	m_CubeMaterial->diffuse = glm::vec3(0.1406f, 0.f, 0.96875f);
	m_CubeMaterial->specular = glm::vec3(0.4726f, 0.8984f, 0.9921f);


	ShaderFilePath shader_file_path
	{
		"Assets/Shaders/NewShadowMappingScene/Lighting.vert", //vertex shader
		"Assets/Shaders/NewShadowMappingScene/Lighting.frag", //fragment shader
	};
	m_Shader.Create("mesh_shader", shader_file_path);
	m_ShaderHotReload.TrackShader(&m_Shader);

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
	m_DirLight.enable = true;

	////////////////////////////////////////
	// Point Light
	////////////////////////////////////////
	m_PointLight.position = glm::vec3(7.5f, 3.0f, 0.0f);
	m_PointLight.enable = false;



	////////////////////////////////////////
	// Shadow 
	////////////////////////////////////////
	m_DirShadowMap.Generate(2048);
	m_DirLightShadowProp.sampleWorldPos = glm::vec3(-6.0f, 10.0f, 0.0f);
	m_DirLightShadowProp.cam_offset = 0.1f;
	m_DirLightShadowProp.config.cam_near = 0.1f;
	m_DirLightShadowProp.config.cam_far = 97.0f;
	m_DirLightShadowProp.config.cam_size = 16.0f;
	m_DirLightShadowProp.UpdateProjMat();
	m_DirLightShadowProp.UpdateViewMatrix(m_DirLightShadowProp.sampleWorldPos,
		m_DirLight.direction, m_DirLightShadowProp.cam_offset);
	m_DirLightShadowProp.debugPara = false;

	m_PointShadowCubeMap.Generate(1024, 1024);

	ShaderFilePath point_shadow_shader_file_path
	{
		"Assets/Shaders/ShadowMapping/ShadowDepthVertex.glsl", //vertex shader
		"Assets/Shaders/ShadowMapping/ShadowDepthFrag.glsl", //fragment shader
		"Assets/Shaders/ShadowMapping/ShadowDepthGeometry.glsl", //geometry shader
	};

	m_ShadowDepthShader.Create("shadow_depth", point_shadow_shader_file_path);


	////Skybox
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

void ShadowMappingScene::DrawObjects(Shader& shader, bool only_geometry)
{
	shader.Bind();
	//plane 
	if (!only_geometry && m_QuadMaterial)
		MaterialShaderHelper(shader, *m_QuadMaterial);

	//Draw planes
	for (const auto& model : m_PlaneTransform)
	{
		shader.SetUniformMat4f("u_Model", model);
		m_SceneRenderer.DrawMesh(m_QuadMesh);

	}

	//draw model
	for (const auto& model : m_ModelTransform)
	{
		shader.SetUniformMat4f("u_Model", model);
		m_SceneRenderer.DrawMesh(m_ModelMesh);
	}


	//sphere
	if (!only_geometry && m_SphereMaterial)
		MaterialShaderHelper(shader, *m_SphereMaterial);

	//Draw sphere
	for (const auto& model : m_SphereTransform)
	{
		shader.SetUniformMat4f("u_Model", model);
		m_SceneRenderer.DrawMesh(m_SphereMesh);
	}



	//cube
	if (!only_geometry && m_CubeMaterial)
		MaterialShaderHelper(shader, *m_CubeMaterial);
	
	//Draw sphere
	for (const auto& model : m_CubeTransform)
	{
		shader.SetUniformMat4f("u_Model", model);
		m_SceneRenderer.DrawMesh(m_CubeMesh);
	}


}

void ShadowMappingScene::ShadowPass()
{
	RenderCommand::CullFront();
	m_ShadowDepthShader.Bind();
	//Directional Ligth shadow
	m_ShadowDepthShader.SetUniform1i("u_IsOmnidir", 0);
	m_ShadowDepthShader.SetUniformMat4f("u_LightSpaceMat", m_DirLightShadowProp.GetLightSpaceMatrix());

	m_DirShadowMap.Write();
	RenderCommand::ClearDepthOnly();
	//Draw object/mesh in scene
	DrawObjects(m_ShadowDepthShader, true);
	m_DirShadowMap.UnBind();


	 //m_ShadowDepthShader.Bind();
	//Point Light shadow 
	std::vector<glm::mat4> shadowMats = PointShadowCalculation::PointLightSpaceMatrix(m_PointLight.position, m_PointShadowConfig);
	//general value
	m_ShadowDepthShader.SetUniform1i("u_IsOmnidir", 1);
	m_ShadowDepthShader.SetUniform1f("u_FarPlane", m_PointShadowConfig.cam_far);

	m_ShadowDepthShader.SetUniformVec3("u_LightPos", m_PointLight.position);
	for (int f = 0; f < 6; ++f)
	{
		m_ShadowDepthShader.SetUniformMat4f(("u_ShadowMatrices[" + std::to_string(f) + "]").c_str(), shadowMats[f]);
	}
	m_PointShadowCubeMap.Write();
	RenderCommand::ClearDepthOnly();
	//Draw object/mesh in scene
	DrawObjects(m_ShadowDepthShader, true);
	m_PointShadowCubeMap.UnBind();


	m_ShadowDepthShader.UnBind();
	RenderCommand::CullBack();
	b_FrameHasShadow = true;
}

void ShadowMappingScene::UpdateCameraUBO(Camera& cam, float aspect_ratio)
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

void ShadowMappingScene::MaterialShaderHelper(Shader& shader, const BaseMaterial& mat)
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

void ShadowMappingScene::UpdateShaders()
{
	m_Shader.Bind();

	//------------Skybox-----------------------
	m_Shader.SetUniform1i("u_UseSkybox", b_EnableSkybox);
	//m_Skybox.ActivateMap(0);
	//m_Shader.SetUniform1i("u_SkyboxTex", 0);


	//------------Shadow-----------------------
	m_Shader.SetUniform1i("u_FrameHasShadow", b_FrameHasShadow);
	if (b_FrameHasShadow)
	{
		m_Shader.SetUniformMat4f("u_LightSpaceMat", m_DirLightShadowProp.GetLightSpaceMatrix());
		m_DirShadowMap.Read(0);
		m_Shader.SetUniform1i("u_DirShadowMap", 0);

		m_PointShadowCubeMap.Read(2);
		m_Shader.SetUniform1i("u_PointShadowCube", 2);
		m_Shader.SetUniform1f("u_OmniShadowFar", m_PointShadowConfig.cam_far);
	}



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


}

void ShadowMappingScene::DebugScene()
{
	//test directional Shadow info 
	if (m_DirLightShadowProp.debugPara)
	{
		auto& ds = m_DirLightShadowProp;
		float dcv = ds.cam_offset + ds.config.cam_near * 0.5f; //dcv is the center/value between the near & far plane 
		glm::vec3 orthCamPos = ds.sampleWorldPos + (m_DirLight.direction * ds.cam_offset);
		glm::vec3 farPlane = orthCamPos + (glm::normalize(-m_DirLight.direction) * ds.config.cam_far);
		glm::vec3 nearPlane = orthCamPos + (glm::normalize(m_DirLight.direction) * ds.config.cam_near);
		DebugGizmos::DrawOrthoCameraFrustrm(orthCamPos, m_DirLight.direction,
			ds.config.cam_near, ds.config.cam_far, ds.config.cam_size,
			glm::vec3(0.0f, 1.0f, 0.0f));

		//Shadow Camera Sample Position 
		DebugGizmos::DrawCross(ds.sampleWorldPos);
		//DebugGizmos::DrawLine(playerTest.aabb.GetCenter(), dirLightObject.sampleWorldPos, glm::vec3(1.0f, 0.0f, 0.0f), playerTest.debugThick);
	}

	if(m_PointShadowConfig.debugLight)
		DebugGizmos::DrawWireSphere(m_PointLight.position, m_PointLight.CalculateLightRadius(0.02f), m_PointLight.colour);


	if (m_PtLightAnimateProp.debugGizmos)
	{
		DebugGizmos::DrawCross(m_PtLightAnimateProp.orbitOrigin);
		DebugGizmos::DrawWireDisc(m_PtLightAnimateProp.orbitOrigin, glm::vec3(1.0f, 0.0f, 0.0f), 
								  glm::vec3(0.0f, 0.0f, 1.0f), m_PtLightAnimateProp.orbitRadius, 
								  20, glm::vec3(0.6f, 0.6f, 0.0f));

		glm::vec3 p = m_PointLight.position;
		p.y = m_PtLightAnimateProp.orbitOrigin.y;
		DebugGizmos::DrawSphere(p, 0.3f, glm::vec3(0.6f, 0.6f, 0.0f));

		//first two sphere
		for (int i = 0; i < 2; i++)
		{
			glm::vec3 p = m_SphereTransform[i][3];
			//m_PtLightAnimateProp.Update(p, m_AnimateObjectInstanceAngle[i+1], 1.0f);
			p.y = m_PtLightAnimateProp.orbitOrigin.y;

			DebugGizmos::DrawSphere(p, 0.3f, glm::vec3(0.6f, 0.6f, 0.0f));
		}
	}

	DebugGizmos::DrawAllBatches();
}

void ShadowMappingScene::MainSceneUIEditor(bool& update_dir_shadow)
{
	if (ImGui::Begin("Shadow Mapping Scene"))
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
				glm::mat4 new_proj = m_Camera->CalculateProjMatrix(window->GetAspectRatio());

			ImGui::TreePop();
		}



		Ext_EnvPropUIPanelSection();
		Ext_LightingUIPanelSection(update_dir_shadow);


		ImGui::End();
	}
}

void ShadowMappingScene::Ext_LightingUIPanelSection(bool& update_dir_shadow)
{
	//////////////////////////////////////
	// Directional Light
	//////////////////////////////////////
	ImGui::Spacing();
	ImGui::SeparatorText("Directional Light");
	ImGui::Checkbox("Enable", &m_DirLight.enable);
	update_dir_shadow |= ImGui::DragFloat3("Light Direction", &m_DirLight.direction[0], 0.1f, -1.0f, 1.0f);
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
}

void ShadowMappingScene::SceneObjectTransformEditorUI()
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

		if (ImGui::TreeNode("Bunny"))
		{
			for (auto& transform : m_ModelTransform)
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

bool ShadowMappingScene::Ext_TransformEditorPanel(glm::mat4& transform)
{
	bool update = false;

	glm::vec3 translate;
	glm::vec3 euler;
	glm::vec3 scale;
	MathsHelper::DecomposeTransform(transform, translate, euler, scale);
	update |= ImGui::DragFloat3("Position", &translate[0], 0.2f);
	update |= ImGui::DragFloat3("Euler", &euler[0], 0.2f);
	update |= ImGui::DragFloat3("Scale", &scale[0], 0.2f);

	if (update)
		transform = glm::translate(glm::mat4(1.0f), translate) *
		glm::mat4_cast(glm::quat(glm::radians(euler))) *
		glm::scale(glm::mat4(1.0f), scale);

	return false;
}

void ShadowMappingScene::ShadowPropEditorUI(bool& update_dir_shadow)
{
	if (ImGui::Begin("Shadow Property Editor"))
	{
		auto& shadow_prop = m_DirLightShadowProp;
		ImGui::Checkbox("Show Debug Gizmos", &shadow_prop.debugPara);
		if (shadow_prop.debugPara && !b_DebugScene)
			ImGui::Checkbox("Enable Scene Debugger First!!!", &b_DebugScene);

		update_dir_shadow |= ImGui::DragFloat3("Sample Location", &shadow_prop.sampleWorldPos[0]);
		update_dir_shadow |= ImGui::DragFloat("Cam offset", &shadow_prop.cam_offset);
		update_dir_shadow |= ImGui::DragFloat("Cam Near", &shadow_prop.config.cam_near);
		update_dir_shadow |= ImGui::DragFloat("Cam Far", &shadow_prop.config.cam_far);
		update_dir_shadow |= ImGui::DragFloat("Cam Size", &shadow_prop.config.cam_size);

		ImGui::SeparatorText("OmniShadow Properties");
		ImGui::PushID(&m_PointShadowConfig);
		ImGui::Checkbox("Show Debug Gizmos", &m_PointShadowConfig.debugLight);
		if (m_PointShadowConfig.debugLight && !b_DebugScene)
			ImGui::Checkbox("Enable Scene Debugger First!!!", &b_DebugScene);
		ImGui::DragFloat("Cam Near", &m_PointShadowConfig.cam_near, 0.1f, 0.0f);
		ImGui::DragFloat("Cam Far", &m_PointShadowConfig.cam_far, 0.1f, 0.0f);
		ImGui::PopID();


		ImGui::SeparatorText("Directional Light Shadow Output");
		static int scale = 1;
		ImGui::SliderInt("image scale", &scale, 1, 5);
		ImVec2 img_size(500.0f * scale, 500.0f * scale);
		ImGui::Image((ImTextureID)(intptr_t)m_DirShadowMap.GetColourAttachment(), 
					img_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::End();
	}
}

void ShadowMappingScene::AnimatedObjectPropertiesEditorUI()
{
	if (ImGui::Begin("Animated Object Properties Editor UI"))
	{
		ImGui::DragFloat3("Orbit Origin", &m_PtLightAnimateProp.orbitOrigin[0], 0.1f);
		ImGui::DragFloat("Angular Speed", &m_PtLightAnimateProp.omega);
		ImGui::DragFloat("Orbit Radius", &m_PtLightAnimateProp.orbitRadius);
		ImGui::Checkbox("Debug Gizmos", &m_PtLightAnimateProp.debugGizmos);
		if (m_PtLightAnimateProp.debugGizmos && !b_DebugScene)
			ImGui::Checkbox("Enable Scene Debugger First!!!", &b_DebugScene);
		ImGui::End();
	}
}

void ShadowMappingScene::Ext_EnvPropUIPanelSection()
{
	ImGui::Spacing();
	ImGui::SeparatorText("Environment");
	ImGui::Checkbox("Enable Skybox", &b_EnableSkybox);
	ImGui::Checkbox("Debug scene", &b_DebugScene);
}
