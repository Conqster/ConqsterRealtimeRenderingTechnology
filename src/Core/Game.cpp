#include "Game.h"
#include "GLFW/glfw3.h"
#include "EventHandle.h"

//#include <direct.h>
#include <iostream>

#include "Graphics/Texture.h"
#include "Graphics/Meshes/CubeMesh.h"
#include "Graphics/Meshes/SquareMesh.h"
#include "Graphics/Meshes/SphereMesh.h"

#include "External Libs/imgui/imgui.h"
#include "External Libs/imgui/imgui_impl_glfw.h"
#include "External Libs/imgui/imgui_impl_opengl3.h"

#include "Graphics/Lights/PointLight.h"


//std::string LightTypeToString(Light light)
//{
//
//}

std::string LightTypeToString(Light light)
{
	//return "hfgb uibguhbu f";
	switch (light.GetType())
	{
		case Point: return "Point Light";
		case Directional: return "Directional Light";
	}
	return "[Error]: Type not configured or null";
}

Game::Game()
{
	TimeTaken constructGame("Construct Game");
	m_Renderer = Renderer(windowWidth, windowHeight);
	std::cout << "a game has been contructed!!!!\n";

	PointLight point_light;
	//Light light;

	std::cout << "End light Constructions\n";
}


void Game::Start()
{
	TimeTaken startTime("Start Game");
	m_Running = m_Renderer.Init();


	if (m_Running)
	{
		m_Camera = Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 10.0f, 0.5f);
	
		//char buffer[FILENAME_MAX];
		//_getcwd(buffer, FILENAME_MAX);
		//std::cout << "Current path: " << buffer << std::endl;

		m_Renderer.CreateGlobalShaderFromFile("src/ShaderFiles/VertexShader.glsl", "src/ShaderFiles/FragmentShader.glsl");
		CreateGameObjects();
		m_Renderer.CreateProjectionViewMatrix();

		m_GameTime.Init(100);


		//IMGUI Stuffs
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui_ImplGlfw_InitForOpenGL(m_Renderer.GetWindow(), true);
		//ImGui::StyleColorsDark();
		ImGui_ImplOpenGL3_Init("#version 400"); //or version 430
	}
}

void Game::Update()
{	
	m_GameTime.Update();
	//m_PhysicsWorld.Update(m_GameTime.DeltaTime() /** 5.0f*//*50.0f*/);
	//TimeTaken updateTime("Update Time");
	//std::cout << "FPS:" << m_GameTime.FPS() << std::endl;


	//RENDERING & GUI STUFFS
	Input();


	if (*m_Renderer.LockCursorFlag())
		m_Camera.Rotate(EventHandle::MousePosition(), (float)m_Renderer.GetWidth(), (float)m_Renderer.GetHeight());
		//m_Camera.Rotate(EventHandle::MouseXChange(), EventHandle::MouseYChange());

	glm::mat4 camera_view = m_Camera.CalViewMat();
	m_Renderer.ClearScreen();


	//static float rotation = 0.0f;
	//float rot_speed = 90.0f;
	//int last_object = m_GameObjects.size() - 1;
	//float current_rot = m_GameObjects[last_object]->rotation.w;
	//m_GameObjects[last_object]->rotation.w += rot_speed * m_GameTime.DeltaTime();
	////m_GameObjects[last_object]->rotation.w = (int)(current_rot + rot_speed * m_GameTime.DeltaTime()) % 360;



	//if (m_SelectedGameobjectidx < m_GameObjects.size())
	//{
	//	auto light_bulb_location = (m_Renderer.GetPointLight() + selected_light)->Ptr_Position();
	//	glm::vec3 selected_obj_location = m_GameObjects[m_SelectedGameobjectidx]->worldPos;

	//	*light_bulb_location = selected_obj_location.x;
	//	*(light_bulb_location + 1) = selected_obj_location.y;
	//	*(light_bulb_location + 2) = selected_obj_location.z;
	//}
		



	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();



	//IMGUI - WINDOW
	{

		//ImGui::ShowDemoWindow();

		ImGui::Begin("Debug");    

		//CAMERA
		ImGui::SeparatorText("Camera info");
		ImGui::Text("Position x: %f, y: %f, z: %f",
					m_Camera.GetPosition().x,
					m_Camera.GetPosition().y,
					m_Camera.GetPosition().z);

		ImGui::Text("Pitch: %f", m_Camera.Ptr_Pitch());
		ImGui::Text("Yaw: %f", m_Camera.Ptr_Yaw());

		ImGui::SliderFloat("Move Speed", m_Camera.Ptr_MoveSpeed(), 5.0f, 50.0f);


		//LIGHT
		//TO-DO: going to create a vector of vector of different lights
		// so to track: 1 directional light
		//				multiple point lights
		//				multiple spot lights
		ImGui::SeparatorText("Lights");
		//auto selected_light = m_Renderer.GetLights()[selected_light_idx] /*+ selected_light_idx*/;
	if(m_Renderer.GetLights().size() > 0)
		if (auto selected_light = m_Renderer.GetLights()[selected_light_idx])
		{
			ImGui::Text("Light: %d - %s", selected_light_idx, selected_light->LightTypeToString());
			ImGui::ColorEdit3("Ambient Light", selected_light->Ptr_Colour());
			ImGui::SliderFloat("Ambient Intensity", selected_light->Ptr_AmbientIntensity(), 0.0f, 1.0f);
			ImGui::SliderFloat("Diffuse Intensity", selected_light->Ptr_DiffuseIntensity(), 0.0f, 1.0f);

			
			if (auto point_light = dynamic_cast<PointLight*>(selected_light))
			{
				ImGui::DragFloat3("Light Pos", point_light->Ptr_Position(), 0.1f);
				ImGui::SliderFloat("Constant", &point_light->Attenuation()[0], 0.0f, 1.0f);
				ImGui::SliderFloat("Linear", &point_light->Attenuation()[1], 0.0f, 1.0f);
				ImGui::SliderFloat("Quadratic", &point_light->Attenuation()[2], 0.0f, 1.0f);

				if (auto spot_light = dynamic_cast<SpotLight*>(selected_light))
				{
					ImGui::SliderFloat3("Direction", spot_light->Ptr_Direction(), -1.0f, 1.0f);
					ImGui::SliderFloat("Falloff", spot_light->Ptr_Falloff(), 0.0f, 5.0f);
				}
			}
			else if (auto directional_light = dynamic_cast<DirectionalLight*>(selected_light))
			{
				ImGui::SliderFloat3("Direction", directional_light->Ptr_Direction(), -1.0f, 1.0f);
				ImGui::SameLine();
				ImGui::Text("Need to normalise this direction!!!!!");
			}

			ImGui::Checkbox("Disable", selected_light->Ptr_Disable());
		}
		else
			ImGui::Text("No Lights!!!!");


		//GAMEOBJECT
		ImGui::SeparatorText("Selected GameObject");
		auto& gameobject = m_GameObjects[m_SelectedGameobjectidx];
		ImGui::DragFloat3("position", &gameobject->worldPos.x, 0.1f);
		ImGui::DragFloat3("Scale", &gameobject->worldScale.x, 0.1f);
		ImGui::SliderFloat4("New Rot", &gameobject->rotation.x, -1.0f, 1.0f);

		SphereMesh* gameobject_mesh = dynamic_cast<SphereMesh*>(gameobject->GetMesh());
		if (gameobject_mesh)
		{
			ImGui::SeparatorText("Sphere Mesh Properties");
			bool update_sector_count = ImGui::SliderInt("Sector Count", &gameobject_mesh->SectorCount, 8, 72);
			bool update_span_count = ImGui::SliderInt("Span Count", &gameobject_mesh->SpanCount, 4, 24);
			ImGui::SeparatorText("Info");
			ImGui::Text("Vertex Count: %d\nIndex Count: %d", 
						gameobject_mesh->GetVerticesCount(),
						gameobject_mesh->GetIndicesCount());

			//ImGui::Text("Index counts: %d", )

			if (update_sector_count || update_span_count)
				gameobject_mesh->Update();
		}

		if(ImGui::Checkbox("Lock Cursor", m_Renderer.LockCursorFlag()))
		{
			//TO-DO: Below is not ideal but might chnage later / dispose
			*m_Renderer.LockCursorFlag() = !(*m_Renderer.LockCursorFlag());
			m_Renderer.ToggleLockCursor();
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
		ImGui::Text("Game Time: %f", m_GameTime.DeltaTime());
		ImGui::Text("Current Index: %d", m_SelectedGameobjectidx);


		//quich dump - for degugging light
		for (auto& light : m_Renderer.GetLights())
		{
			ImGui::Text("Light- %s", light->LightTypeToString());
			if (auto spot_light = dynamic_cast<SpotLight*>(light))
			{
				ImGui::Text("Direction X: %f, Y: %f, Z: %f", spot_light->GetDirection().x, spot_light->GetDirection().y, spot_light->GetDirection().z);
				ImGui::Text("Falloff: %f", spot_light->GetFalloff());
			}
		}

		ImGui::End();


	}


	
	m_Renderer.RenderObjects(m_GameObjects, camera_view);
	//m_GlobalShaderProgram.SetUniformMat4f("u_view", camera_view);


	//m_Renderer.RenderObjects();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	m_Renderer.SwapOpenGLBuffers();
	//Graphics::UpdateWindowTitle(m_GameTime.FPS());

	//std::cout << "game updating.......................\n";

	//DELETE STUFFS !!!!!!!!!!!!!!!!!
	if (m_GameObjects.size() > 0)
	{
		static float delete_objects_cooldown;
		if (EventHandle::GetKeys()[GLFW_KEY_Y] && delete_objects_cooldown <= 0)
		{
			delete_objects_cooldown = 0.5f;
			delete m_GameObjects[m_SelectedGameobjectidx];
			m_GameObjects.erase(m_GameObjects.begin() + m_SelectedGameobjectidx);

			//if (m_SelectedGameobjectidx != 0)
			//	m_SelectedGameobjectidx--;
			//else
			//	m_SelectedGameobjectidx = (m_SelectedGameobjectidx + 1) % m_GameObjects.size();

			if (m_SelectedGameobjectidx > m_GameObjects.size() - 1)
				m_SelectedGameobjectidx --;
		}
		else
			delete_objects_cooldown -= m_GameTime.DeltaTime();
	}
}

void Game::End()
{
	TimeTaken EndGame("Close Game");
	std::cout << "Game ended!!!!!!!!!\n";

	//Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	m_Renderer.CloseWindow();
}

bool Game::Running()
{
	return m_Running;
}

void Game::Input()
{
	glfwPollEvents();
	bool* keys = EventHandle::GetKeys();

	bool program_should_close = m_Renderer.WindowShouldClose();



	if (keys[GLFW_KEY_ESCAPE] || program_should_close)
	{
		m_Running = false;
	}

	//int state = EventHandle::GetKeyState(Graphics::GetWindow(), GLFW_KEY_T);
	static float cooldown; 
	if (keys[GLFW_KEY_T] && cooldown <= 0)
	{
		cooldown = 0.2f;
		m_Camera.SetPosition(glm::vec3());
	}
	else
		cooldown -= m_GameTime.DeltaTime();

	static float lock_cooldown;
	if (keys[GLFW_KEY_L] && lock_cooldown <= 0)
	{
		lock_cooldown = 0.2f;
		m_Renderer.ToggleLockCursor();
	}
	else
		lock_cooldown -= m_GameTime.DeltaTime();


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


		auto& selected_gameobject = m_GameObjects[m_SelectedGameobjectidx];
		if (selected_gameobject)
		{
			float sensitivity = 0.4f;
			x *= sensitivity;
			y *= sensitivity;

			selected_gameobject->worldPos += glm::vec3(x, y, 0.0f);
			selected_gameobject->worldScale += glm::vec3(1.0f) * re_scale;
		}
	}




	//MOVE CAMERA
	if (keys[GLFW_KEY_W])
	{
		m_Camera.Translate(m_Camera.GetFroward(), m_GameTime.DeltaTime());
	}
	if (keys[GLFW_KEY_S])
	{
		m_Camera.Translate(m_Camera.GetFroward() * -1.0f, m_GameTime.DeltaTime());
	}
	if (keys[GLFW_KEY_A])
	{
		m_Camera.Translate(m_Camera.GetRight() * -1.0f, m_GameTime.DeltaTime());

	}
	if (keys[GLFW_KEY_D])
	{
		m_Camera.Translate(m_Camera.GetRight(), m_GameTime.DeltaTime());
	}
	if (keys[GLFW_KEY_E])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT])
			m_Camera.Translate(m_Camera.GetUp(), m_GameTime.DeltaTime());
		else
			m_Camera.Translate(glm::vec3(0.0f, 1.0f, 0.0f), m_GameTime.DeltaTime());
	}
	if (keys[GLFW_KEY_Q])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT])
			m_Camera.Translate(m_Camera.GetUp() * -1.0f, m_GameTime.DeltaTime());
		else
			m_Camera.Translate(glm::vec3(0.0f, -1.0f, 0.0f), m_GameTime.DeltaTime());
	}



	//GAMEOBJECT SELECTOR 
	unsigned int gameobject_count = m_GameObjects.size();
	if (gameobject_count > 0)
	{
		static float selector_cooldown;
		if (keys[GLFW_KEY_4] && selector_cooldown <= 0)
		{

			if (m_SelectedGameobjectidx < m_GameObjects.size())
			{
				m_GameObjects[m_SelectedGameobjectidx]->selected = false;

				selector_cooldown = 0.4f;
				if (m_SelectedGameobjectidx != 0)
					m_SelectedGameobjectidx--;
				else
					m_SelectedGameobjectidx = gameobject_count - 1;


				m_GameObjects[m_SelectedGameobjectidx]->selected = true;
			}

		}
		else
			selector_cooldown -= m_GameTime.DeltaTime();

		if (keys[GLFW_KEY_6] && selector_cooldown <= 0)
		{
			if (m_SelectedGameobjectidx < m_GameObjects.size())
			{
				m_GameObjects[m_SelectedGameobjectidx]->selected = false;

				selector_cooldown = 0.4f;
				m_SelectedGameobjectidx = (m_SelectedGameobjectidx + 1) % gameobject_count;

				m_GameObjects[m_SelectedGameobjectidx]->selected = true;
			}
		}
		else
			selector_cooldown -= m_GameTime.DeltaTime();
	}


	//////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////FORCE LIGHT LOCATION/////////////////////
	//////////////////////////////////////////////////////////////////////////////////


	static float toogle_cooldown;
	if (keys[GLFW_KEY_O] && toogle_cooldown < 0)
	{
		toogle_cooldown = 0.2f;
		selected_light_idx = (selected_light_idx + 1) % m_Renderer.LightsCount();
	}
	else
		toogle_cooldown -= m_GameTime.DeltaTime();

}

void Game::CreateGameObjects()
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
	ground->worldPos = glm::vec3(0.0f, /*-150.0f*/ -3.0f,  0.0f);
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


	m_SelectedGameobjectidx = 0;
	if (m_GameObjects.size() > 0)
		m_GameObjects[m_SelectedGameobjectidx]->selected = true;
}
