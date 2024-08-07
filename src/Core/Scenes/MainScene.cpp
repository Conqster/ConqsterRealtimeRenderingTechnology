#include "MainScene.h"

#include "EventHandle.h"
#include "External Libs/imgui/imgui.h"

//#include "Graphics/Material.h"
#include "Graphics/Meshes/SphereMesh.h"
#include "Graphics/Meshes/CubeMesh.h"
#include "Graphics/Meshes/SquareMesh.h"
#include "Graphics/Texture.h"

void MainScene::SetWindow(Window* window)
{
	this->window = window;
}

void MainScene::OnInit()
{
	//TO:DO: take this opengl func out later into child classes 
	//		 so each child could define their specific behaviour 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 1.0f/*0.5f*/);

	NewShaderFilePath shader_file{ "src/ShaderFiles/New/MainSceneVertexShader.glsl" ,"src/ShaderFiles/New/MainSceneFragShader.glsl" };
	m_MainShaderProgram.Create("main_shader", shader_file);

	CreateObjects();
	SetupLights();
}

void MainScene::OnUpdate(float delta_time)
{
	//Input for now
	ProcessInput(delta_time);

	OnRender();
}

void MainScene::OnRender()
{
	//TO-DO: might re-define this if i want to run multiple scene at the same time
	glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_MainShaderProgram.Bind();
	//this does not support the new shader yet
	//m_MainShaderProgram.SetUniform1i("u_UseNew", 0);

	//Process Camera
	m_MainShaderProgram.SetUniformMat4f("u_projection", m_Camera->CalculateProjMatrix(window->GetAspectRatio()));
	m_MainShaderProgram.SetUniformMat4f("u_view", m_Camera->CalViewMat());
	m_MainShaderProgram.SetUniformVec3("u_ViewPos", m_Camera->GetPosition());
	// 
	//Process Light
	ProcessLight(m_MainShaderProgram);

	//Process GameObjects 
	glm::mat4 gameobject_model;
	for (size_t i = 0; i < m_GameObjects.size(); i++)
	{
		auto& gameobject = m_GameObjects[i];
		gameobject_model = glm::mat4(1.0f);
		gameobject_model = glm::translate(gameobject_model, gameobject->worldPos);
		//first 3 == xyz
		gameobject_model = glm::rotate(gameobject_model, glm::radians(gameobject->rotation.w), (glm::vec3)gameobject->rotation);
		gameobject_model = glm::scale(gameobject_model,/* 50.0f **/ gameobject->worldScale);

		m_MainShaderProgram.SetUniformMat4f("u_model", gameobject_model);

		bool debug_mode = false;
		//DEBUGING GAMEOBJECT
		if (gameobject->selected)
		{
			m_MainShaderProgram.SetUniform1i("u_DebugMode", 1);
			//gameobject->Draw(); //solid debug
			gameobject->DebugDraw(); //line debug
			m_MainShaderProgram.SetUniform1i("u_DebugMode", 0);
		}
		else if (debug_mode)
		{
			//gameobject->Draw();
			m_MainShaderProgram.SetUniform1i("u_DebugMode", 1);
			//gameobject->Draw(); //solid debug
			gameobject->DebugDraw(); //line debug
			m_MainShaderProgram.SetUniform1i("u_DebugMode", 0);
		}


		gameobject->Draw();
	}


	//for debugging light pos	
	for (auto& l : m_LightManager.GetLights())
	{
		if (auto pt = std::dynamic_pointer_cast<NewPointLight>(l))
		{
			glm::mat4 model12 = glm::mat4(1.0f);
			model12 = glm::translate(model12, pt->position);
			model12 = glm::scale(model12, glm::vec3(0.25f));
			m_MainShaderProgram.SetUniformMat4f("u_model", model12);
			m_GameObjects[2]->Draw();
		}

	}



	//modelShader.UseShader();
	//modelShader.SetUniformMat4f("u_View", camera.CalViewMat());
	//modelShader.SetUniformVec3("u_ViewPos", camera.GetPosition());
	//modelShader.SetUniform1f("u_GlobalAmbientStrength", test_GlobalAmbientStrength);
	//modelShader.SetUniform1f("u_GlobalDiffuseStrength", test_GlobalDiffuseStrength);
	//modelShader.SetUniform1f("u_GlobalSpecularStrength", test_GlobalSpecularStrength);
	//modelShader.SetUniformMat4f("u_Projection", camera.CalculateProjMatrix(m_Window->GetAspectRatio()));
	//glm::mat4 model1 = glm::mat4(1.0f);
	//model1 = glm::translate(model1, glm::vec3(0.0f));
	////model1 = glm::rotate(model1, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	////model1 = glm::scale(model1, glm::vec3(5.05f));
	//modelShader.SetUniform1i("u_Material.shininess", test_Material.shiniess);
	//modelShader.SetUniform1f("u_Material.emissionIntensity", newMaterialEmissionIntensity);
	//modelShader.SetUniformMat4f("u_Model", model1);
	////Dir light
	//modelShader.SetUniform1i("u_DirectionalLight.lightBase.use", dir_Light.lightBase.use);
	//modelShader.SetUniformVec3("u_DirectionalLight.lightBase.ambient", dir_Light.lightBase.ambient);
	//modelShader.SetUniformVec3("u_DirectionalLight.lightBase.diffuse", dir_Light.lightBase.diffuse);
	//modelShader.SetUniformVec3("u_DirectionalLight.lightBase.specular", dir_Light.lightBase.specular);
	//modelShader.SetUniformVec3("u_DirectionalLight.direction", dir_Light.direction);
	//newModel.Draw(modelShader);

}

void MainScene::OnRenderUI()
{
	ImGui::Begin("Debug");

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

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////SCENE PROPERTIES//////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	ImGui::SeparatorText("Scene Properties");

	if (ImGui::Checkbox("Lock Cursor", window->Ptr_LockCursorFlag()))
	{
		//TO-DO: Below is not ideal but might chnage later / dispose
		*window->Ptr_LockCursorFlag() = !(*window->Ptr_LockCursorFlag());
		window->ToggleLockCursor();
	}
	ImGui::ColorEdit3("clear Screen Colour", &m_ClearScreenColour[0]);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////LIGHT//////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	ImGui::SeparatorText("Lights");
	ImGui::Text("Light counts: %d", m_LightManager.GetLights().size());
	ImGui::Text("Global values");
	ImGui::SliderInt("Object Specular Exponent", &g_ObjectSpecularExponent, 0, 256);
	ImGui::SliderFloat("Ambient Intensity", &g_AbientStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse Intensity", &g_DiffuseStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Intensity", &g_SpecularStrength, 0.0f, 1.0f);
	auto& selected_light = m_LightManager.GetLights()[selected_light_idx];
	if (m_LightManager.GetLights().size() > 0)
		if (selected_light)
		{
			ImGui::PushID(&selected_light);
			ImGui::Text("Light: %d - %s", selected_light_idx, selected_light->LightTypeToString());
			ImGui::ColorEdit3("Ambient Colour", &selected_light->ambient[0]);
			ImGui::ColorEdit3("Diffuse Colour", &selected_light->diffuse[0]);
			ImGui::ColorEdit3("Specular Colour", &selected_light->specular[0]);


			if (auto point_light = std::dynamic_pointer_cast<NewPointLight>(selected_light))
			{
				ImGui::DragFloat3("Light Pos", &point_light->position[0], 0.1f);
				ImGui::SliderFloat("Constant", &point_light->attenuation[0], 0.0f, 1.0f);
				ImGui::SliderFloat("Linear", &point_light->attenuation[1], 0.0f, 1.0f);
				ImGui::SliderFloat("Quadratic", &point_light->attenuation[2], 0.0f, 1.0f);

				if (auto spot_light = std::dynamic_pointer_cast<NewSpotLight>(selected_light))
				{
					ImGui::SliderFloat3("Direction", &spot_light->direction[0], -1.0f, 1.0f);
					ImGui::SliderFloat("Inner Cutoff Angle", &spot_light->innerCutoffAngle, 0.0f, 60.0f);
					ImGui::SliderFloat("Outer Cutoff Angle", &spot_light->outerCutoffAngle, 0.0f, 60.0f);
				}
			}
			else if (auto directional_light = std::dynamic_pointer_cast<NewDirectionalLight>(selected_light))
			{
				ImGui::SliderFloat3("Direction", &directional_light->direction[0], -1.0f, 1.0f);
				ImGui::SameLine();
				ImGui::Text("Need to normalise this direction!!!!!");
			}

			ImGui::Checkbox("Enable", &selected_light->enable);
			ImGui::PopID();
		}
		else
			ImGui::Text("No Lights!!!!");




	//GAMEOBJECT
	ImGui::SeparatorText("Selected GameObject");
	auto& gameobject = m_GameObjects[m_CurrentSelectedGameobjectIdx];
	ImGui::DragFloat3("position", &gameobject->worldPos.x, 0.1f);
	ImGui::DragFloat3("Scale", &gameobject->worldScale.x, 0.1f);
	ImGui::SliderFloat4("New Rot", &gameobject->rotation.x, -1.0f, 1.0f);

	auto mesh = gameobject->GetMesh();
	if (mesh && ImGui::TreeNode("Mesh Vertices Info"))
	{
		ImGui::Text("Vertex Count: %d", mesh->GetVerticesCount());
		if (ImGui::TreeNode("Vertices Details"))
		{
			auto& v = *mesh->Ptr_Vertices();
			for (unsigned int i = 0; i < v.size(); i++)
			{
				ImGui::PushID(&v[i]);
				ImGui::Text("Vertex %d", i);
				ImGui::Text("  positions (%f, %f, %f, %f)",
					v[i].position[0], v[i].position[1],
					v[i].position[2], v[i].position[3]);
				ImGui::Text("  colour (%f, %f, %f, %f)",
					v[i].colour[0], v[i].colour[1],
					v[i].colour[2], v[i].colour[3]);
				ImGui::Text("  normals (%f, %f, %f)",
					v[i].normals[0], v[i].normals[1],
					v[i].normals[2]);
				ImGui::Text("  uv (%f, %f)",
					v[i].texCoord[0], v[i].texCoord[1]);
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}


	//Material* gameobject_material = gameobject->GetMaterial();
	//if (gameobject_material)
	//{
	//	if (ImGui::TreeNode("GameObject Material"))
	//	{
	//		ImGui::SliderFloat("Metallic", gameobject_material->Ptr_Metallic(), 0.0f, 1.0f, "%.2f");
	//		ImGui::SliderFloat("Smoothness", gameobject_material->Ptr_Smoothness(), 0.0f, 1.0f, "%.2f");

	//		ImGui::TreePop();
	//	}
	//}

	SphereMesh* gameobject_mesh = dynamic_cast<SphereMesh*>(gameobject->GetMesh());
	if (gameobject_mesh)
	{
		if (ImGui::TreeNode("GameObject Mesh"))
		{
			bool update_sector_count = ImGui::SliderInt("Sector Count", &gameobject_mesh->SectorCount, 8, 72);
			bool update_span_count = ImGui::SliderInt("Span Count", &gameobject_mesh->SpanCount, 4, 24);
			ImGui::SeparatorText("Info");
			ImGui::Text("Vertex Count: %d\nIndex Count: %d",
				gameobject_mesh->GetVerticesCount(),
				gameobject_mesh->GetIndicesCount());

			//ImGui::Text("Index counts: %d", )

			if (update_sector_count || update_span_count)
				gameobject_mesh->Update();

			ImGui::TreePop();
		}

	}





	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SeparatorText("Infos");
	ImGui::TextWrapped("	Toggle Cursor - L, This Disables Camera mouse Rotation");
	ImGui::Text("    Toggle between Lights - O, If more than one exists");
	ImGui::Text("	Use WASD - To move Camera Around");
	ImGui::TextWrapped("	Use Q & E - To move Camera Down & Up (World Space) +HOLD: SHIFT (Local Space)");
	ImGui::Text("	Use T - To Reset Camera");
	ImGui::TextWrapped("	Use 4 & 6 - To cycle through gameobject Left - Right Respectively");
	ImGui::Text("	Use Arrow Keys - To move specified target object");
	ImGui::TextWrapped("	Hold Shift + Arrows Keys Up or Down - To Scale Up or Down");
	ImGui::Text("	Use Y - To delete first gameobject in list");

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SeparatorText("Stats");
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	//ImGui::Text("Game Time: %f", del);
	ImGui::Text("Current Index: %d", m_CurrentSelectedGameobjectIdx);
	ImGui::Text("Mouse Pos: %f, %f", window->GetMouseScreenPosition().x, window->GetMouseScreenPosition().y);
	ImGui::Text("Event Mouse Pos: %f, %f", EventHandle::MousePosition().x, EventHandle::MousePosition().y);
	ImGui::Text("Default/Start Screen Size: %d, %d", window->GetWidth(), window->GetHeight());
	ImGui::Text("Default/Start Half Screen Size: %f, %f", window->GetWidth() * 0.5f, window->GetHeight() * 0.5f);



	//quich dump - for degugging light
	for (auto& light : m_LightManager.GetLights())
	{
		ImGui::Text("Light - %s", light->LightTypeToString());
		if (auto spot_light = std::dynamic_pointer_cast<NewSpotLight>(light))
		{
			ImGui::Text("Direction X: %f, Y: %f, Z: %f", spot_light->direction.x, spot_light->direction.y, spot_light->direction.z);
			ImGui::Text("Outer Cutoff: %f", spot_light->outerCutoffAngle);
			ImGui::Text("Inner Cutoff: %f", spot_light->innerCutoffAngle);
		}
	}



	ImGui::End();
}

void MainScene::OnDestroy()
{

	for (auto& game_object : m_GameObjects)
		delete game_object;

	m_GameObjects.clear();

	Scene::OnDestroy();
}

void MainScene::CreateObjects()
{
	//TO-DO: fix bug i cant use the same mesh with two different obj
		// may if its a pointer

		//////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////SHARED MESHES/////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////
	static SphereMesh sphere_mesh;
	static CubeMesh cube_mesh;
	static SquareMesh square_mesh;
	sphere_mesh.Create();
	cube_mesh.Create();
	square_mesh.Create();

	//////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////TEXTURE///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	static Texture brick_texture("Assets/Textures/brick.png");
	static Texture ground_texture("Assets/Textures/plain64.png");
	static Texture image_texture("Assets/Textures/At Manchester.png");


	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////SPHERE//////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	GameObject* obj = new GameObject(&sphere_mesh, &brick_texture);
	//obj->rotation = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	obj->worldPos = glm::vec3(0.0f, 5.0f, 0.0f);
	obj->radius = 1.0f;
	//m_PhysicsWorld.AddBody(obj);
	m_GameObjects.push_back(obj);


	//////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////BOX / CUBE/////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	GameObject* obj2 = new GameObject(&cube_mesh, &/*image_texture*/ground_texture);
	obj2->worldPos = glm::vec3(-6.0f, 2.5f, 0.0f);
	obj2->worldScale = glm::vec3(2.0f);
	//m_PhysicsWorld.AddBody(obj2);
	m_GameObjects.push_back(obj2);


	//////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////SMALL CUBE/////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	GameObject* small_cube = new GameObject(&cube_mesh, &image_texture);
	small_cube->worldPos = glm::vec3(6.0f, 2.5f, 0.0f);
	small_cube->worldScale = glm::vec3(0.5f);
	//m_PhysicsWorld.AddBody(obj2);
	m_GameObjects.push_back(small_cube);






	//////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////GROUND/////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	GameObject* ground = new GameObject(&square_mesh, &ground_texture);
	ground->worldPos = glm::vec3(0.0f, /*-150.0f*/ -3.0f, 0.0f);
	ground->worldScale = glm::vec3(200.0f, 200.0f, 0.1f);
	ground->rotation = glm::vec4(glm::vec3(1.0f, 0.0f, 0.0f), -90.0f);
	//m_PhysicsWorld.AddBody(ground);
	m_GameObjects.push_back(ground);


	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////BIG SPHERE//////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	GameObject* big_ball = new GameObject(&sphere_mesh, &brick_texture);
	//big_ball->worldPos = glm::vec3(15.0f, 0.0f, 0.0f);
	//big_ball->worldPos = glm::vec3(0.0f, -10.0f, 0.0f);
	big_ball->worldScale = glm::vec3(2.5f);
	big_ball->m_Static = true;
	big_ball->radius = 2.5f;
	//m_PhysicsWorld.AddBody(big_ball);
	m_GameObjects.push_back(big_ball);


}

void MainScene::SetupLights()
{
	m_LightManager.Add(std::make_shared<NewDirectionalLight>());

	m_LightManager.Add(std::make_shared<NewPointLight>());
	m_LightManager.Add(std::make_shared<NewPointLight>());
	m_LightManager.Add(std::make_shared<NewPointLight>());

	m_LightManager.Add(std::make_shared<NewSpotLight>());
	m_LightManager.Add(std::make_shared<NewSpotLight>());
	m_LightManager.Add(std::make_shared<NewSpotLight>());


	for (auto dir : m_LightManager.GetDirLights())
		if (auto lock_dir = dir.lock())
			lock_dir->enable = true;


	for (auto pt : m_LightManager.GetPointLights())
		if (auto lock_pt = pt.lock())
			lock_pt->enable = false;

	for (auto sp : m_LightManager.GetSpotLights())
		if (auto lock_sp = sp.lock())
			lock_sp->enable = false;


}

void MainScene::ProcessLight(NewShader& shader)
{
	shader.Bind();
	shader.SetUniform1i("u_ObjectSpecularExponent", g_ObjectSpecularExponent);

	shader.SetUniform1f("u_GlobalAmbientStrength", g_AbientStrength);
	shader.SetUniform1f("u_GlobalDiffuseStrength", g_DiffuseStrength);
	shader.SetUniform1f("u_GlobalSpecularStrength", g_SpecularStrength);
	//Process Directional Light
	auto& dir_light = m_LightManager.GetDirLights();
	//only support one light at the moment
	if (auto dir = dir_light[0].lock())
	{
		shader.SetUniformVec3f("u_DirectionalLight.lightBase.ambient", dir->ambient);
		shader.SetUniformVec3f("u_DirectionalLight.lightBase.diffuse", dir->diffuse);
		shader.SetUniformVec3f("u_DirectionalLight.lightBase.specular", dir->specular);
		shader.SetUniform1i("u_DirectionalLight.lightBase.use", dir->enable);
		shader.SetUniformVec3("u_DirectionalLight.direction", dir->direction);
	}


	//Loop throught point lights 
	auto& pt_lights = m_LightManager.GetPointLights();
	shader.SetUniform1i("u_PointLightCount", pt_lights.size());

	if (pt_lights.size() > 0)
	{
		for (size_t i = 0; i < pt_lights.size(); i++)
		{
			if (auto pt = pt_lights[i].lock())
			{
				//printf("light idx %d, is enable: %d \n", i, pt->enable);
				char light_ambient[64];
				sprintf_s(light_ambient, "u_PointLights[%zu].lightBase.ambient", i);
				shader.SetUniformVec3f(light_ambient, pt->ambient);
				char pt_diffuse[64];
				sprintf_s(pt_diffuse, "u_PointLights[%zu].lightBase.diffuse", i);
				shader.SetUniformVec3f(pt_diffuse, pt->diffuse);
				char pt_specular[64];
				sprintf_s(pt_specular, "u_PointLights[%zu].lightBase.specular", i);
				shader.SetUniformVec3f(pt_specular, pt->specular);
				char pt_enable[64];
				sprintf_s(pt_enable, "u_PointLights[%zu].lightBase.use", i);
				shader.SetUniform1i(pt_enable, pt->enable);
				char pt_position[64];
				sprintf_s(pt_position, "u_PointLights[%zu].position", i);
				shader.SetUniformVec3(pt_position, pt->position);
				char pt_attenuation[64];
				sprintf_s(pt_attenuation, "u_PointLights[%zu].attenuation", i);
				shader.SetUniformVec3f(pt_attenuation, pt->attenuation);
			}
		}
	}





	//Loop throught spot lights 
	auto& sp_lights = m_LightManager.GetSpotLights();
	shader.SetUniform1i("u_SpotLightCount", sp_lights.size());

	if (sp_lights.size() > 0)
	{
		for (size_t i = 0; i < sp_lights.size(); i++)
		{
			if (auto sp = sp_lights[i].lock())
			{
				//printf("light idx %d, is enable: %d \n", i, sp->enable);
				char light_ambient[64];
				sprintf_s(light_ambient, "u_SpotLights[%zu].pointLightBase.lightBase.ambient", i);
				shader.SetUniformVec3f(light_ambient, sp->ambient);
				char sp_diffuse[64];
				sprintf_s(sp_diffuse, "u_SpotLights[%zu].pointLightBase.lightBase.diffuse", i);
				shader.SetUniformVec3f(sp_diffuse, sp->diffuse);
				char sp_specular[64];
				sprintf_s(sp_specular, "u_SpotLights[%zu].pointLightBase.lightBase.specular", i);
				shader.SetUniformVec3f(sp_specular, sp->specular);
				char sp_enable[64];
				sprintf_s(sp_enable, "u_SpotLights[%zu].pointLightBase.lightBase.use", i);
				shader.SetUniform1i(sp_enable, sp->enable);
				char sp_position[64];
				sprintf_s(sp_position, "u_SpotLights[%zu].pointLightBase.position", i);
				shader.SetUniformVec3(sp_position, sp->position);
				char sp_attenuation[64];
				sprintf_s(sp_attenuation, "u_SpotLights[%zu].pointLightBase.attenuation", i);
				shader.SetUniformVec3f(sp_attenuation, sp->attenuation);

				char sp_direction[64];
				sprintf_s(sp_direction, "u_SpotLights[%zu].direction", i);
				shader.SetUniformVec3(sp_direction, sp->direction);
				char sp_innerAngle[64];
				sprintf_s(sp_innerAngle, "u_SpotLights[%zu].innerCutoffAngle", i);
				shader.SetUniform1f(sp_innerAngle, glm::cos(glm::radians(sp->innerCutoffAngle)));
				char sp_outerAngle[64];
				sprintf_s(sp_outerAngle, "u_SpotLights[%zu].outerCutoffAngle", i);
				shader.SetUniform1f(sp_outerAngle, glm::cos(glm::radians(sp->outerCutoffAngle)));
			}
		}
	}

}

void MainScene::ProcessInput(float delta_time)
{
	bool* keys = EventHandle::GetKeys();

	static float toogle_cooldown;
	if (keys[GLFW_KEY_O] && toogle_cooldown < 0)
	{
		toogle_cooldown = 0.2f;
		selected_light_idx = (selected_light_idx + 1) % m_LightManager.GetLights().size();
	}
	else
		toogle_cooldown -= delta_time;



	//TRANSLATE SELECTED GAMEOBJECT
	if (m_GameObjects.size() > 0)
	{
		float x = 0;
		float y = 0;
		float re_scale = 0;
		if (!keys[GLFW_KEY_LEFT_SHIFT] && keys[GLFW_KEY_UP])
		{
			y += 1;
		}
		else if (!keys[GLFW_KEY_LEFT_SHIFT] && keys[GLFW_KEY_DOWN])
		{
			y -= 1;
		}

		if (keys[GLFW_KEY_RIGHT])
		{
			x -= 1;
		}
		else if (keys[GLFW_KEY_LEFT])
		{
			x += 1;
		}

		if (keys[GLFW_KEY_LEFT_SHIFT] && keys[GLFW_KEY_UP])
		{
			re_scale += 0.1f;
		}
		else if (keys[GLFW_KEY_LEFT_SHIFT] && keys[GLFW_KEY_DOWN])
		{
			re_scale -= 0.1f;
		}


		auto& selected_gameobject = m_GameObjects[m_CurrentSelectedGameobjectIdx];
		if (selected_gameobject)
		{
			float sensitivity = 0.4f;
			x *= sensitivity;
			y *= sensitivity;

			selected_gameobject->worldPos += glm::vec3(x, y, 0.0f);
			selected_gameobject->worldScale += glm::vec3(1.0f) * re_scale;
		}
	}



	{
		//GAMEOBJECT SELECTOR 
		unsigned int gameobject_count = m_GameObjects.size();
		if (gameobject_count > 0)
		{
			static float selector_cooldown;
			if (keys[GLFW_KEY_4] && selector_cooldown <= 0)
			{

				if (m_CurrentSelectedGameobjectIdx < m_GameObjects.size())
				{
					m_GameObjects[m_CurrentSelectedGameobjectIdx]->selected = false;

					selector_cooldown = 0.4f;
					if (m_CurrentSelectedGameobjectIdx != 0)
						m_CurrentSelectedGameobjectIdx--;
					else
						m_CurrentSelectedGameobjectIdx = gameobject_count - 1;


					m_GameObjects[m_CurrentSelectedGameobjectIdx]->selected = true;
				}

			}
			else
				selector_cooldown -= delta_time;

			if (keys[GLFW_KEY_6] && selector_cooldown <= 0)
			{
				if (m_CurrentSelectedGameobjectIdx < m_GameObjects.size())
				{
					m_GameObjects[m_CurrentSelectedGameobjectIdx]->selected = false;

					selector_cooldown = 0.4f;
					m_CurrentSelectedGameobjectIdx = (m_CurrentSelectedGameobjectIdx + 1) % gameobject_count;

					m_GameObjects[m_CurrentSelectedGameobjectIdx]->selected = true;
				}
			}
			else
				selector_cooldown -= delta_time;
		}

	}

}

