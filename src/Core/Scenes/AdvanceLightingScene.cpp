#include "AdvanceLightingScene.h"

#include "SceneGraph/Model.h"
#include "SceneGraph/ModelLoader.h"
#include "Graphics/RendererErrorAssertion.h"

#include "External Libs/imgui/imgui.h"
#include "Util/FilePaths.h"


void AdvanceLightingScene::SetWindow(Window* window)
{
	this->window = window;
}

void AdvanceLightingScene::OnInit(Window* window)
{
	Scene::OnInit(window);

	window->UpdateProgramTitle("Advance Lighting Scene");

	glEnable(GL_DEPTH_TEST);

	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 1.0f/*0.5f*/);

	CreateObjects();
}

void AdvanceLightingScene::OnUpdate(float delta_time)
{
	OnRender();
}

void AdvanceLightingScene::OnRender()
{
	GLCall(glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//Camera uniform GPU buffer update
	m_CameraMatUBO.Bind();
	m_CameraMatUBO.SetBufferSubData(0, sizeof(glm::mat4), &(m_Camera->CalculateProjMatrix(window->GetAspectRatio())[0][0]));
	m_CameraMatUBO.SetBufferSubData(sizeof(glm::mat4), sizeof(glm::mat4), &(m_Camera->CalViewMat()[0][0]));
	m_CameraMatUBO.UnBind();

	/////////////////////
	// First Pass : Draw Scene
	/////////////////////
	DrawObjects(modelShader);

	/////////////////////
	// Second Pass : Draw Debug normal
	/////////////////////
	debugShader.Bind();
	debugShader.SetUniform1f("u_NorDebugLength", normDebugLength);
	debugShader.SetUniformVec3("u_DebugColour", normDebugColour);
	debugShader.SetUniform1i("u_UseDebugColour", useDebugColour);
	debugShader.SetUniform1i("u_DebugPosColour", debugVertexPosColour);
	DrawObjects(debugShader);
	

	//model2 = glm::translate(model2, glm::vec3(0.0f, 1.0f, 0.0f));
	//modelShader.SetUniformMat4f("u_Model", model2);
	//sphere.Render();
	//sphereTex->DisActivate();
	//modelShader.UnBind();

	////////////////
	// SECOND PASS ON SPHERE FOR DEBUGGING ITS NORMAL
	////////////////

}

void AdvanceLightingScene::OnRenderUI()
{
	ImGui::Begin("Advance Lighting Scene");

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
	ImGui::SeparatorText("Model 1");
	ImGui::DragFloat3("Model 1 pos", &model_1Pos[0], 0.1f);
	ImGui::SliderFloat("Model 1 Scale", &model_1Scale, 0.1f, 30.0f);
	ImGui::SeparatorText("Model 2");
	ImGui::DragFloat3("Model 2 pos", &model_2Pos[0], 0.1f);
	ImGui::SliderFloat("Model 2 Scale", &model_2Scale, 0.01f, 20.0f);

	ImGui::Spacing();

	ImGui::SeparatorText("normal debugging");
	ImGui::Checkbox("Use debug colour", &useDebugColour);
	ImGui::Checkbox("Debug vertex pos colour", &debugVertexPosColour);
	ImGui::ColorEdit3("norm Debug colour", &normDebugColour[0]);
	ImGui::SliderFloat("Debug length", &normDebugLength, -/*1.0f*/ 0.0f, 2.0f, "%.2f");

	ImGui::End();
}

void AdvanceLightingScene::OnDestroy()
{
}

void AdvanceLightingScene::CreateObjects()
{
	///////////////
	// Create model
	///////////////
	model_1 = modelLoader.Load(FilePaths::Instance().GetPath("electrical-charger"), true);
	model_2 = modelLoader.Load(FilePaths::Instance().GetPath("bunny"), true);
	//model_1 = modelLoader.Load("Assets/Textures/backpack/backpack.obj", true);


	//model shader
	ShaderFilePath shader_file_path
	{
		"src/ShaderFiles/Learning/AdvanceLighting/ModelVertex.glsl", //vertex shader
		"src/ShaderFiles/Learning/AdvanceLighting/ModelFrag.glsl", //fragment shader
	};

	modelShader.Create("model_shader", shader_file_path);



	////////////////////////////////////////
	// CREATE CAMERA MAT UNIFORM BUFFER
	////////////////////////////////////////
	long long int buf_size = 2 * sizeof(glm::mat4) + sizeof(glm::vec2);   //to store view, projection & screenRes(vec2)
	m_CameraMatUBO.Generate(buf_size);

	////////////////////////////////////////
	// CREATE SPHERE MESH
	////////////////////////////////////////
	sphere.Create();
	

	////////////////////////////////////////
	// CREATE TEXTURES 
	////////////////////////////////////////
	//brick texture 
	brickTex = new Texture(FilePaths::Instance().GetPath("brick"));
	//plain texture
	plainTex = new Texture(FilePaths::Instance().GetPath("plain"));



	//////////////////////////////////////
	// CREATE GROUND MESH
	//////////////////////////////////////
	ground.Create();


	//DEBUG SHADER
	ShaderFilePath debug_sphere_shader_file
	{
		"src/ShaderFiles/Learning/Geometry/VertexDebugNormal.glsl", //vertex shader
		"src/ShaderFiles/Learning/Geometry/LineFragment.glsl", //frag shader
		"src/ShaderFiles/Learning/Geometry/GeometryDebugNormal.glsl"  //geometry shader
	};

	debugShader.Create("debug_norm_shader", debug_sphere_shader_file);

}

void AdvanceLightingScene::DrawObjects(Shader& shader)
{
	//ground 
	shader.Bind();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(20.0f));
	shader.SetUniformMat4f("u_Model", model);
	brickTex->Activate();
	//ground 1
	ground.Render();
	//ground 2
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shader.SetUniformMat4f("u_Model", model);
	ground.Render();
	//ground 3
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ground.Render();
	brickTex->DisActivate();
	//modelShader.UnBind();


	//modelShader.Bind();
	model = glm::mat4(1.0f);
	model = glm::translate(model, model_1Pos);
	model = glm::scale(model, glm::vec3(1.0f) * model_1Scale);
	shader.SetUniformMat4f("u_Model", model);
	//sphereTex->Activate();
	model_1->Draw();


	//modelShader.Bind();
	plainTex->Activate();
	model = glm::mat4(1.0f);
	model = glm::translate(model, model_2Pos);
	model = glm::scale(model, glm::vec3(1.0f) * model_2Scale);
	shader.SetUniformMat4f("u_Model", model);
	//model_2->Draw(modelShader);
	model_2->Draw();
	//model_2->DebugWireDraw();
	plainTex->DisActivate();
	shader.UnBind();
}
