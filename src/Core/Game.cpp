#pragma once
#include "Game.h"

#include "GLFW/glfw3.h"
#include "EventHandle.h"

#include "Camera.h"
#include <windows.h>

#include "Scene Graph/Scenes/MainScene.h"
#include "Scene Graph/Scenes/Light&ModelScene.h"
#include "Scene Graph/Scenes/AdvanceOpenGLScene.h"
#include "Scene Graph/Scenes/FaceCullingScene.h"
#include "Scene Graph/Scenes/Textures_FrameBufferScene.h"
#include "Scene Graph/Scenes/GeometryScene.h"
#include "Scene Graph/Scenes/InstancingScene.h"
#include "Scene Graph/Scenes/AntiAliasingScene.h"
//#include "Scene Graph/Scenes/AdvanceLightingScene.h"
//#include "Scene Graph/Scenes/ParallaxExperimentalScene.h"
#include "Scene Graph/Scenes/ExperimentScene.h"

//FOR TESTING: Remove later
#include "External Libs/imgui/imgui.h"
#include "Util/FilePaths.h"


void Game::OnStart()
{
	TimeTaken InitGame("Init Game");

	m_Time.Init(100);


	m_WindowProp.width = 1000;
	m_WindowProp.height = 700;

	if (m_UseFullScreen)
		m_WindowProp.width = GetSystemMetrics(SM_CXSCREEN), m_WindowProp.height = GetSystemMetrics(SM_CYSCREEN);



	m_Window = new Window(m_WindowProp);


	/////////////////////////
	// FILE PATHS
	/////////////////////////
	{
		TimeTaken FilePathRegistation("File Path Registeration");
		//Register file path
		FilePaths::Instance().RegisterPath("brick", "Assets/Textures/brick.png");
		FilePaths::Instance().RegisterPath("manchester-image", "Assets/Textures/At Manchester.jpg");
		FilePaths::Instance().RegisterPath("container", "Assets/Textures/container.png");
		FilePaths::Instance().RegisterPath("container-specular", "Assets/Textures/container_specular.png");
		FilePaths::Instance().RegisterPath("dirt", "Assets/Textures/dirt.png");
		FilePaths::Instance().RegisterPath("grass", "Assets/Textures/grass.png");
		FilePaths::Instance().RegisterPath("marble", "Assets/Textures/marble.jpeg");
		FilePaths::Instance().RegisterPath("metal", "Assets/Textures/metal.jpeg");
		FilePaths::Instance().RegisterPath("plain", "Assets/Textures/plain64.png");
		FilePaths::Instance().RegisterPath("blank-image", "Assets/Textures/BlankPlane.png");
		FilePaths::Instance().RegisterPath("floor-brick-diff", "Assets/Textures/floor_brick/patterned_brick_floor_diff.jpg");
		FilePaths::Instance().RegisterPath("floor-brick-nor", "Assets/Textures/floor_brick/patterned_brick_floor_nor.jpg");

		FilePaths::Instance().RegisterPath("para-brick-diff", "Assets/Textures/Parallax/bricks.jpg");
		FilePaths::Instance().RegisterPath("para-brick-nor", "Assets/Textures/Parallax/bricks_normal.jpg");
		FilePaths::Instance().RegisterPath("para-brick-disp", "Assets/Textures/Parallax/bricks_disp.jpg");

		FilePaths::Instance().RegisterPath("cobblestone-diff", "Assets/Textures/cobblestone/patterned_cobblestone_02_diff_4k.jpg");
		FilePaths::Instance().RegisterPath("cobblestone-nor", "Assets/Textures/cobblestone/patterned_cobblestone_02_nor_gl_4k.jpg");
		FilePaths::Instance().RegisterPath("cobblestone-disp", "Assets/Textures/cobblestone/patterned_cobblestone_02_disp_4k.jpg");
		
		FilePaths::Instance().RegisterPath("old_plank", "Assets/Textures/old_planks_diff.jpg");
		FilePaths::Instance().RegisterPath("glass", "Assets/Textures/glass-background-with-frosted-pattern.jpg");



		FilePaths::Instance().RegisterPath("bunny", "Assets/Models/stanford-bunny.obj");
		FilePaths::Instance().RegisterPath("backpack", "Assets/Models/backpack/backpack.obj");
		FilePaths::Instance().RegisterPath("shapes", "Assets/Models/blendershapes/blend_shapes.obj");
		FilePaths::Instance().RegisterPath("electrical-charger", "Assets/Textures/sci-fi_electrical_charger/scene.gltf");
	}




	//Register scenes
	m_SceneManager = new SceneManager();
	//m_SceneManager->RegisterNewScene<MainScene>("Main Scene");
	m_SceneManager->RegisterNewScene<Texture_FrameBufferScene>("Texture_FrameBufferScene");
	//m_SceneManager->RegisterNewScene<Light_ModelScene>("Light Model");
	//m_SceneManager->RegisterNewScene<AdvanceOpenGLScene>("Advance Scene");
	m_SceneManager->RegisterNewScene<FaceCullingScene>("Face Culling");
	m_SceneManager->RegisterNewScene<GeometryScene>("Geometry Scene");
	//m_SceneManager->RegisterNewScene<InstancingScene>("Instance Scene");
	m_SceneManager->RegisterNewScene<AntiAliasingScene>("AntiAliasing Scene");
	//m_SceneManager->RegisterNewScene<AdvanceLightingScene>("Advance Lighting Scene");
	//m_SceneManager->RegisterNewScene<ParallaxExperimentalScene>("Parallax Scene");
	m_SceneManager->RegisterNewScene<ExperimentScene>("ReWorking_Scene_Rendering");

	//Load
	//m_CurrentScene = m_SceneManager->LoadScene("Instance Scene", m_Window);
	//m_CurrentScene = m_SceneManager->LoadScene("Advance Lighting Scene", m_Window);
	//m_CurrentScene = m_SceneManager->LoadScene("Parallax Scene", m_Window);
	m_CurrentScene = m_SceneManager->LoadScene("ReWorking_Scene_Rendering", m_Window);
	//m_CurrentScene = m_SceneManager->LoadScene("Face Culling", m_Window);
	//m_CurrentScene = m_SceneManager->LoadScene("Texture_FrameBufferScene", m_Window);
	//m_CurrentScene = m_SceneManager->LoadScene("AntiAliasing Scene", m_Window);
	//m_CurrentScene = m_SceneManager->LoadScene("Main Scene", m_Window);







	//Sample to get file path with key
	std::cout << FilePaths::Instance().GetPath("bunny") << "\n";

	if (m_Window)
	{
		
		//UIManager
		m_UI = new UIManager(*m_Window);
		//m_UI->OnInit(*m_Window);

		m_Running = true;
		m_GameState = State::LOADSCENE;

		if (m_CurrentScene)
		{
			//m_CurrentScene->OnInit(m_Window);
			m_GameState = State::RUNNINGSCENE;
		}


		std::cout << "/////////////////////////////////////////////////////////////////////\n";
		std::cout << "LOAD COMPLETION\n";
		std::cout << "/////////////////////////////////////////////////////////////////////\n";

		return;
	}
	std::cout << "Failed to init window &/ Scene!!!!!\n";
}

void Game::OnLoadSceneUI(const char* label, bool can_load)
{
	/////////////////////////////////////////////
	// UI - ImGui
	// Later create a sub ui panel, to abstract out ImGui function from Game.
	/////////////////////////////////////////////
	ImGui::Begin(label);

	static int cur_sel = 0;
	ImGui::Combo("Scenes", &cur_sel, m_SceneManager->ScenesByNamePtr(), m_SceneManager->SceneCount());

	if (ImGui::Button(label))
	{
		if (!can_load)
		{
			m_CurrentScene->OnDestroy();
			m_CurrentScene = nullptr;
		}

		//m_CurrentScene = m_SceneManager->LoadScene(m_SceneManager->ScenesByName()[cur_sel], m_Window);
		m_CurrentScene = m_SceneManager->LoadScene(m_SceneManager->ScenesByNamePtr()[cur_sel], m_Window);
		if (m_CurrentScene)
			m_GameState = State::RUNNINGSCENE;

	}


	ImGui::SeparatorText("Stats");
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::SameLine();
	ImGui::Checkbox("VSync", m_Window->GetVSync());
	//ImGui::Text("Game Time: %f", del);
	ImGui::Text("Mouse Pos: %f, %f", m_Window->GetMouseScreenPosition().x, m_Window->GetMouseScreenPosition().y);
	ImGui::Text("Event Mouse Pos: %f, %f", EventHandle::MousePosition().x, EventHandle::MousePosition().y);
	ImGui::Text("Screen Size: %d, %d", m_Window->GetWidth(), m_Window->GetHeight());

	ImGui::End();



	
}


void Game::Run()
{



	while (m_Running)
	{

		m_Time.Update();

		Input();

		switch (m_GameState)
		{
		case State::LOADSCENE:

			m_Window->OnWaitUpdate();

			m_UI->OnStartFrame();
			OnLoadSceneUI("load scene", true);
			m_UI->OnEndFrame();
			//m_Window->OnUpdate();
			//std::cout << "Loading scene\n";

			break;
		case State::RUNNINGSCENE:
		
		
			m_CurrentScene->OnUpdate(m_Time.DeltaTime());

			//std::cout << "running scene\n";
			m_UI->OnStartFrame();
			m_CurrentScene->OnRenderUI();
			OnLoadSceneUI("switch scene", false);
			m_UI->OnEndFrame();

			m_Window->OnUpdate();

			break;
		}

	}

	OnEnd();
}

void Game::OnEnd()
{
	TimeTaken ShuttingDown("Shutting down program");

	if (m_CurrentScene)
	{
		m_CurrentScene->OnDestroy();
		delete m_CurrentScene;
		m_CurrentScene = nullptr;
	}


	if (m_UI)
	{
		m_UI->OnDestroy();
		delete m_UI;
		m_UI = nullptr;
	}

	if (m_Window)
	{
		m_Window->Close();
		delete m_Window;
		m_Window = nullptr;
	}
}


void Game::Input()
{
	bool* keys = EventHandle::GetKeys();
	bool (*mouse_buttons)[3] = EventHandle::GetMouseButton();
	//bool(*key_codes)[3] = EventHandle::GetKeyCodes();



	bool program_should_close = m_Window->WindowShouldClose();


	if ((keys[GLFW_KEY_LEFT_ALT] && keys[GLFW_KEY_F4]) || program_should_close)
		m_Running = false;

	//if (keys[GLFW_KEY_ESCAPE] || program_should_close)
	//{
	//	m_Running = false;
	//}

	//int state = EventHandle::GetKeyState(Graphics::GetWindow(), GLFW_KEY_T);

	if(m_CurrentScene)
	{
		static float cooldown;
		if (keys[GLFW_KEY_T] && cooldown <= 0)
		{
			cooldown = 0.2f;
			m_CurrentScene->GetCamera()->SetPosition(glm::vec3());
		}
		else
			cooldown -= m_Time.DeltaTime();

		static float lock_cooldown;
		if (keys[GLFW_KEY_ESCAPE] && lock_cooldown <= 0)
		{
			lock_cooldown = 0.2f;
			m_Window->ToggleLockCursor();
		}
		else
			lock_cooldown -= m_Time.DeltaTime();



		//if (*m_Window->Ptr_LockCursorFlag())
		//	m_CurrentScene->GetCamera()->Rotate(EventHandle::MousePosition(), (float)m_Window->GetWidth(), (float)m_Window->GetHeight());

		if (*m_Window->Ptr_LockCursorFlag())
			m_CurrentScene->GetCamera()->Rotate(EventHandle::MousePosition(), (float)m_Window->GetWidth(), (float)m_Window->GetHeight());
		//m_CurrentScene->GetCamera()->Rotate(EventHandle::MouseXChange(), EventHandle::MouseYChange());
		else
		{
			if (mouse_buttons[GLFW_MOUSE_BUTTON_LEFT][GLFW_RELEASE] && !*m_UI->ImGuiWantCaptureMouse())
				m_Window->ToggleLockCursor();
		}



		//MOVE CAMERA
		if (keys[GLFW_KEY_W])
		{
			m_CurrentScene->GetCamera()->Translate(m_CurrentScene->GetCamera()->GetForward(), m_Time.DeltaTime());
		}
		if (keys[GLFW_KEY_S])
		{
			m_CurrentScene->GetCamera()->Translate(m_CurrentScene->GetCamera()->GetForward() * -1.0f, m_Time.DeltaTime());
		}
		if (keys[GLFW_KEY_A])
		{
			m_CurrentScene->GetCamera()->Translate(m_CurrentScene->GetCamera()->GetRight() * -1.0f, m_Time.DeltaTime());

		}
		if (keys[GLFW_KEY_D])
		{
			m_CurrentScene->GetCamera()->Translate(m_CurrentScene->GetCamera()->GetRight(), m_Time.DeltaTime());
		}
		if (keys[GLFW_KEY_E])
		{
			if (keys[GLFW_KEY_LEFT_SHIFT])
				m_CurrentScene->GetCamera()->Translate(m_CurrentScene->GetCamera()->GetUp(), m_Time.DeltaTime());
			else
				m_CurrentScene->GetCamera()->Translate(glm::vec3(0.0f, 1.0f, 0.0f), m_Time.DeltaTime());
		}
		if (keys[GLFW_KEY_Q])
		{
			if (keys[GLFW_KEY_LEFT_SHIFT])
				m_CurrentScene->GetCamera()->Translate(m_CurrentScene->GetCamera()->GetUp() * -1.0f, m_Time.DeltaTime());
			else
				m_CurrentScene->GetCamera()->Translate(glm::vec3(0.0f, -1.0f, 0.0f), m_Time.DeltaTime());
		}
	}
	
}






