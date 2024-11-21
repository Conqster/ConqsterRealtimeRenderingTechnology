#include "InstancingScene.h"

#include "External Libs/imgui/imgui.h"
#include "Renderer/Meshes/PrimitiveMeshFactory.h"

void InstancingScene::OnInit(Window* window)
{
	Scene::OnInit(window);

	window->UpdateProgramTitle("Instancing Scene");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 35.0f, 1.0f/*0.5f*/);

	CreateObjects();
}

void InstancingScene::OnUpdate(float delta_time)
{
	OnRender();
}

void InstancingScene::OnRender()
{
	//TO-DO: might re-define this if i want to run multiple scene at the same time
	glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //Not using the stencil buffer now


	///////////////////////////////////////////
	// FIRST RENDER PASS
	///////////////////////////////////////////

	//Camera GPU buffer update
	glm::mat4 view = m_Camera->CalViewMat();
	glm::vec2 screen_res = glm::vec2(window->GetWidth(), window->GetHeight());
	float aspect_ratio = window->GetAspectRatio();
	glm::mat4 proj = m_Camera->CalculateProjMatrix(window->GetAspectRatio());
	m_CameraMatUBO.Bind();
	m_CameraMatUBO.SetBufferSubData(&proj[0][0], sizeof(glm::mat4), 0);
	m_CameraMatUBO.SetBufferSubData(&view[0][0], sizeof(glm::mat4), sizeof(glm::mat4));
	m_CameraMatUBO.SetBufferSubData(&screen_res[0], sizeof(glm::vec2), 2 * sizeof(glm::mat4));
	m_CameraMatUBO.UnBind();

	//Render square 
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3());
	model = glm::scale(model, glm::vec3(1.0f) * squareScale);

	m_Shader.Bind();
	//instances
	glm::vec2 translate_offsets[100];
	int idx = 0;
	//float offset = 10.1f;
	//for(int y = -10; y < 10; y += 2)
	//	for (int x = -10; x < 10; x += 2)
	//	{
	//		glm::vec2 trans;
	//		trans.x = (float)x / 10.0f + offset;
	//		trans.y = (float)y / 10.0f + offset;
	//		translate_offsets[idx++] = trans;
	//	}

	int count = instanceCount;
	glm::vec2 origin = glm::vec2(0.0f);
	glm::vec2 pos_offset = glm::vec2(0.0f);

	int test = 0;

	for (int i = 0; i < count; i++)
	{
		pos_offset.x = (float)i * offsetInstance;
		for (int j = 0; j < 5; j++)
		{
			if (idx >= count)
				break;

			pos_offset.y = (float)j * offsetInstance;
			//translate_offsets[idx++] = origin + pos_offset;
			translate_offsets[idx++] = origin + pos_offset;
		}
	}

	for (int i = 0; i < count; i++)
		m_Shader.SetUniformVec2(("offsets[" + std::to_string(i) + "]"), translate_offsets[i]);

	//m_SquareMesh.GetVAO()->Bind();
	m_Shader.SetUniformMat4f("u_Model", model);
	//m_Shader.SetUniform1f("u_AR", aspect_ratio);
	m_Shader.SetUniform1f("u_Width", (float)window->GetWidth());
	m_Shader.SetUniform1f("u_Height", (float)window->GetHeight());
	m_Shader.SetUniform1i("u_Debug", debugColour);
	//m_SquareMesh.Render();
	//m_SquareMesh.RenderInstances(100);
	//m_SquareMesh.RenderInstances(idx);
	m_SceneRenderer.DrawMeshInstance(m_SquareMesh, idx);

	//Render sphere with square shader
	//sphereMesh.RenderInstances(idx);
	m_SceneRenderer.DrawMeshInstance(sphereMesh, idx);

	m_Shader.UnBind();

	modelShader.Bind();
	model = glm::mat4(1.0f);
	//model = glm::scale(model, glm::vec3(1.0f) * 0.1f);
	model = glm::scale(model, glm::vec3(1.0f) * modelScale);
	//model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
	samplePlainTexture->Activate();
	modelShader.SetUniformMat4f("u_Model", model);
	modelShader.SetUniform1i("u_Debug", debugColour);
	sampleModel->Draw(modelShader);
	samplePlainTexture->DisActivate();
	modelShader.UnBind();


	////////////////
	// SECOND PASS ON MODEL FOR DEBUGGING ITS NORMAL
	////////////////
	debugNorShader.Bind();
	debugNorShader.SetUniform1f("u_NorDebugLength", normDebugLength);
	debugNorShader.SetUniformVec3("u_DebugColour", normDebugColour);
	debugNorShader.SetUniform1i("u_UseDebugColour", useDebugColour);
	debugNorShader.SetUniformMat4f("u_Model", model);
	sampleModel->Draw();
	debugNorShader.UnBind();

	//SKYBOX
	m_Skybox.Draw(*m_Camera, *window);
}

void InstancingScene::OnRenderUI()
{

	ImGui::Begin("Debug Instancing Scene");

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



	///////////////////////////////////
	// SCENE SPECIIFC
	///////////////////////////////////
	ImGui::Spacing();
	ImGui::SeparatorText("Scene Properties");
	ImGui::Checkbox("Debug", &debugColour);
	ImGui::SliderFloat("Square Scale", &squareScale, 0.0f, 1.0f, "%.2f");
	ImGui::SliderInt("Instance Count", &instanceCount, 1, 20);
	ImGui::SliderFloat("Offset Instance", &offsetInstance, 0.0f, 10.0f, "%.2f");


	//DEBUGGER
	ImGui::Spacing();
	ImGui::SeparatorText("normal debugging");
	ImGui::Checkbox("Use debug colour", &useDebugColour);
	ImGui::ColorEdit3("Debug colour", &normDebugColour[0]);
	ImGui::SliderFloat("Debug length", &normDebugLength, -1.0f, 2.0f, "%.2f");

	ImGui::Spacing();
	ImGui::SliderFloat("Sample model scale", &modelScale, 0.1f, 50.0f);


	ImGui::End();
}

void InstancingScene::OnDestroy()
{
	m_Shader.Clear();
	m_SquareMesh.Clear();
	m_CameraMatUBO.Delete();
	m_Skybox.Destroy();
	sphereMesh.Clear();
	sampleModel->Destroy();
	modelLoader.Clean();
	modelShader.Clear();
	debugNorShader.Clear();

	samplePlainTexture->UnRegisterUse();
	samplePlainTexture = nullptr;
}

InstancingScene::~InstancingScene()
{
}

void InstancingScene::CreateObjects()
{
	///////////////////////////////////
	// CREATE SHADER
	///////////////////////////////////

	ShaderFilePath shader_file_path
	{
		"Assets/Shaders/Learning/Instancing/Vertex.glsl", //vertex shader
		"Assets/Shaders/Learning/Instancing/Fragment.glsl", //fragment shader
	};

	m_Shader.Create("shader_1", shader_file_path);



	////////////////////////////////////////
	// CREATE SQUARE MESH
	////////////////////////////////////////
	m_SquareMesh = CRRT::PrimitiveMeshFactory::Instance().CreateAQuad();



	////////////////////////////////////////
	// CREATE CAMERA MAT UNIFORM BUFFER
	////////////////////////////////////////
	long long int buf_size = 2 * sizeof(glm::mat4) + sizeof(glm::vec2);   //to store view, projection & screenRes(vec2)
	m_CameraMatUBO.Generate(buf_size);


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



	/////////////////////////////////////////
	//CREATE SPHERE MESH
	/////////////////////////////////////////
	sphereMesh = CRRT::PrimitiveMeshFactory::Instance().CreateASphere();


	/////////////////////////////////////////
	// CREATE MODEL
	/////////////////////////////////////////
	//sampleModel = modelLoader.Load("Assets/Textures/backpack/backpack.obj");
	sampleModel = modelLoader.Load("Assets/Textures/sci-fi_electrical_charger/scene.gltf", true);
	sampleModel = modelLoader.Load("Assets/Models/stanford-bunny.obj", true);


	//Sample Texture
	samplePlainTexture = new Texture("Assets/Textures/plain64.png");

	ShaderFilePath shader_file_path2
	{
		"Assets/Shaders/Learning/Instancing/ModelVertex.glsl", //vertex shader
		"Assets/Shaders/Learning/Instancing/Fragment.glsl", //fragment shader
	};

	modelShader.Create("model_shader", shader_file_path2);


	//SPHERE NORMAL DEBUG SHADER
	ShaderFilePath debug_norm_shader
	{
		"Assets/Shaders/Learning/Geometry/VertexDebugNormal.glsl", //vertex shader
		"Assets/Shaders/Learning/Geometry/LineFragment.glsl", //frag shader
		"Assets/Shaders/Learning/Geometry/GeometryDebugNormal.glsl"  //geometry shader
	};

	debugNorShader.Create("debug_norm_shader", debug_norm_shader);
}
