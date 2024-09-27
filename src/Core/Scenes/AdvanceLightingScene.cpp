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

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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
	//general debug parameters for model shader
	modelShader.Bind();
	modelShader.SetUniform1i("u_DebugScene", debugScene);
	modelShader.SetUniform1i("u_DebugWcType", debugModelType);
	modelShader.SetUniform1i("u_DisableTex", disableTexture);
	LightPass(modelShader);
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

	////////////////////////////////////////////////
	// SCENE OBJECTS
	////////////////////////////////////////////////
	ImGui::Spacing();
	if (ImGui::TreeNode("Scene Objects"))
	{
		ImGui::SeparatorText("Ground");
		ImGui::SliderFloat("Ground Scale", &groundScale, 100.0f, 1000.0f);

		//ImGui::SeparatorText("Model 1");
		//ImGui::DragFloat3("Model 1 pos", &model_1Pos[0], 0.1f);
		//ImGui::SliderFloat("Model 1 Scale", &model_1Scale, 0.1f, 30.0f);

		if (ImGui::TreeNode("Bunny Model"))
		{
			for (int i = 0; i < MAX_BUNNY_MODEL; i++)
			{
				std::string label = "Bunny model index " + std::to_string(i);
				//ImGui::PushID(&cubesPos[i]);
				ImGui::SeparatorText(label.c_str());
				ImGui::DragFloat3((label + " pos").c_str(), &bunnysPos[i][0], 0.1f);
				ImGui::SliderFloat((label + " scale").c_str(), &bunnysScale[i], 0.1f, 30.f);
				//ImGui::PopID();
			}
			ImGui::TreePop();
		}

		//ImGui::SeparatorText("Sphere");
		//ImGui::DragFloat3("Sphere pos", &spherePos[0], 0.1f);
		//ImGui::SliderFloat("Sphere scale", &sphereScale, 0.1f, 10.0f);

		if (ImGui::TreeNode("Spheres"))
		{
			for (int i = 0; i < MAX_SPHERE; i++)
			{
				std::string label = "sphere index " + std::to_string(i);
				//ImGui::PushID(&cubesPos[i]);
				ImGui::SeparatorText(label.c_str());
				ImGui::DragFloat3((label + " pos").c_str(), &spheresPos[i][0], 0.1f);
				ImGui::SliderFloat((label + " scale").c_str(), &spheresScale[i], 0.1f, 10.f);
				//ImGui::PopID();
			}
			ImGui::TreePop();
		}


		//ImGui::SeparatorText("Cube");
		//ImGui::DragFloat3("Cube pos", &cubePos[0], 0.1f);
		//ImGui::SliderFloat("Cube scale", &cubeScale, 0.1f, 10.0f);


		if (ImGui::TreeNode("Cubes"))
		{
			for (int i = 0; i < MAX_CUBE; i++)
			{
				std::string label = "cube index " + std::to_string(i);
				//ImGui::PushID(&cubesPos[i]);
				ImGui::SeparatorText(label.c_str());
				ImGui::DragFloat3((label + " pos").c_str(), &cubesPos[i][0], 0.1f);
				ImGui::SliderFloat((label + " scale").c_str(), &cubesScale[i], 0.1f, 10.f);
				//ImGui::PopID();
			}
			ImGui::TreePop();
		}


		ImGui::SeparatorText("Model 2");
		ImGui::DragFloat3("Model 2 pos", &model_2Pos[0], 0.1f);
		ImGui::SliderFloat("Model 2 Scale", &model_2Scale, 0.01f, 20.0f);


		ImGui::TreePop();
	}


	///////////////////////////////////////////
	// SCENE LIGHTS
	///////////////////////////////////////////
	ImGui::Spacing();
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::SeparatorText("Light Global Properties");
		ImGui::SliderInt("Specular Shinness", &specShinness, 0, 128);
		ImGui::Checkbox("Debug Lights Pos", &debugLightPos);

		ImGui::SeparatorText("Point Light");
		ImGui::DragFloat3("Point Light position", &pointLight.position[0], 0.1f);
		ImGui::ColorEdit3("Point Light colour", &pointLight.colour[0]);

		ImGui::TreePop();
	}


	///////////////////////////////////////////
	// NORMAL DEBUGGING
	///////////////////////////////////////////
	ImGui::Spacing();
	ImGui::SeparatorText("General Debugging");
	ImGui::Checkbox("Debug Scene", &debugScene);
	if (debugScene)
	{
		static int cur_sel = 0;
		const char* element_name[] = {"Model Space", "Model Normal", "Model Abs Normal","Model Colour", "Default Colour"};
		ImGui::Combo("Debug Colour Type", &cur_sel, element_name, IM_ARRAYSIZE(element_name));

		debugModelType = (DebugModelType)cur_sel;
	}
	ImGui::Checkbox("Disable Texture", &disableTexture);
	ImGui::SeparatorText("Normal debugging");
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
	// Create Objects & model
	///////////////
	model_1 = modelLoader.Load(FilePaths::Instance().GetPath("bunny"), true);
	model_2 = modelLoader.Load(FilePaths::Instance().GetPath("backpack"), true);
	//model_1 = modelLoader.Load("Assets/Textures/backpack/backpack.obj", true);

	////////////////////////////////////////
	// CREATE SPHERE MESH
	////////////////////////////////////////
	sphere.Create();

	//////////////////////////////////////
	// CREATE GROUND MESH
	//////////////////////////////////////
	ground.Create();

	//////////////////////////////////////
	// CREATE CUBE MESH
	//////////////////////////////////////
	cube.Create();


	//generate pos&scale for cubes 
	//(9.0f, 1.0f, 5.0f) 
	//move backwards (-Z) by 4 units
	glm::vec3 origin = glm::vec3(9.0f, 1.0f, 5.0f);
	float offset_units = -4.0f;
	for (int i = 0; i < MAX_CUBE; i++)
	{
		cubesPos[i] = origin + glm::vec3(0.0f, 0.0f, offset_units * i);
		cubesScale[i] = 2.0f;
	}

	cubesScale[0] = 100.0f;

	//generate pos&scale for sphere
	//(4.5f, 0.5f, 5.0f)
	//move backwards (-Z) by 4 units
	origin = glm::vec3(4.5f, 1.0f, 5.0f);
	for (int i = 0; i < MAX_SPHERE; i++)
	{
		spheresPos[i] = origin + glm::vec3(0.0f, 0.0f, offset_units * i);
		spheresScale[i] = 1.0f;
	}

	//generate pos&scale for sphere
	//(0.0f, 0.0f, 5.0f)
	//move backwards (-Z) by 4 units
	origin = glm::vec3(0.0f, 0.0f, 5.0f);
	for (int i = 0; i < MAX_BUNNY_MODEL; i++)
	{
		bunnysPos[i] = origin + glm::vec3(0.0f, 0.0f, offset_units * i);
		bunnysScale[i] = 10.0f;
	}

	//////////////////////////////////////
	// GENERATE SHADERS
	//////////////////////////////////////
	//model shader
	ShaderFilePath shader_file_path
	{
		"src/ShaderFiles/Learning/AdvanceLighting/ModelVertex.glsl", //vertex shader
		"src/ShaderFiles/Learning/AdvanceLighting/ModelFrag.glsl", //fragment shader
	};
	modelShader.Create("model_shader", shader_file_path);
	//DEBUG SHADER
	ShaderFilePath debug_sphere_shader_file
	{
		"src/ShaderFiles/Learning/Geometry/VertexDebugNormal.glsl", //vertex shader
		"src/ShaderFiles/Learning/Geometry/LineFragment.glsl", //frag shader
		"src/ShaderFiles/Learning/Geometry/GeometryDebugNormal.glsl"  //geometry shader
	};
	debugShader.Create("debug_norm_shader", debug_sphere_shader_file);



	////////////////////////////////////////
	// CREATE CAMERA MAT UNIFORM BUFFER
	////////////////////////////////////////
	long long int buf_size = 2 * sizeof(glm::mat4) + sizeof(glm::vec2);   //to store view, projection & screenRes(vec2)
	m_CameraMatUBO.Generate(buf_size);


	////////////////////////////////////////
	// CREATE TEXTURES 
	////////////////////////////////////////
	//brick texture 
	brickTex = new Texture(FilePaths::Instance().GetPath("brick"));
	//plain texture
	plainTex = new Texture(FilePaths::Instance().GetPath("plain"));
	//manchester-image
	manchesterTex = new Texture(FilePaths::Instance().GetPath("manchester-image"));




	/////////////////////////////////////////
	// DEFINE LIGHT NECESSARY PROP
	/////////////////////////////////////////
	pointLight.colour = glm::vec3(1.0f, 0.0f, 1.0f);
	pointLight.position = glm::vec3(0.0f, 3.0f, 0.0f);

	debugScene = true;
	debugModelType = MODEL_NORMAL;
}

void AdvanceLightingScene::DrawObjects(Shader& shader)
{
	shader.Bind();

	//ground 
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f) * groundScale);
	shader.SetUniformMat4f("u_Model", model);
	//brickTex->Activate();
	manchesterTex->Activate();
	//ground 1
	ground.Render();
	//brickTex->DisActivate();
	manchesterTex->DisActivate();
	//modelShader.UnBind();


	//BUNNY MODELS
	for (int i = 0; i < MAX_CUBE; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, bunnysPos[i]);
		model = glm::scale(model, glm::vec3(1.0f) * bunnysScale[i]);
		shader.SetUniformMat4f("u_Model", model);
		model_1->Draw();
	}


	plainTex->Activate();
	model = glm::mat4(1.0f);
	model = glm::translate(model, model_2Pos);
	model = glm::scale(model, glm::vec3(1.0f) * model_2Scale);
	shader.SetUniformMat4f("u_Model", model);
	//model_2->Draw(modelShader);
	model_2->Draw();
	//model_2->DebugWireDraw();
	plainTex->DisActivate();



	//SPHERES
	plainTex->Activate();
	for (int i = 0; i < MAX_CUBE; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, spheresPos[i]);
		model = glm::scale(model, glm::vec3(1.0f) * spheresScale[i]);
		shader.SetUniformMat4f("u_Model", model);
		sphere.Render();
	}
	plainTex->DisActivate();



	//CUBES
	//plainTex->Activate();
	manchesterTex->Activate();
	for (int i = 0; i < MAX_CUBE; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, cubesPos[i]);
		model = glm::scale(model, glm::vec3(1.0f) * cubesScale[i]);
		shader.SetUniformMat4f("u_Model", model);
		cube.Render();
	}
	//plainTex->DisActivate();
	manchesterTex->DisActivate();




	shader.UnBind();
}

void AdvanceLightingScene::LightPass(Shader& shader)
{
	shader.Bind();

	shader.SetUniformVec3("u_ViewPos", m_Camera->GetPosition());

	////////////////////////
	// Point Light 
	////////////////////////
	shader.SetUniform1i("u_Shininess", specShinness);
	shader.SetUniformVec3("u_Light.position", pointLight.position);
	shader.SetUniformVec3("u_Light.colour", pointLight.colour);

	shader.SetUniform1i("u_DebugLightLocation", debugLightPos);
	if (debugLightPos)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, pointLight.position);
		model = glm::scale(model, glm::vec3(0.2f));

		shader.SetUniformMat4f("u_Model", model);
		sphere.RenderDebugOutLine();
	}

	shader.SetUniform1i("u_DebugLightLocation", 0);
	shader.UnBind();
}
