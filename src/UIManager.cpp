#pragma once
#include "UIManager.h"

#include "External Libs/imgui/imgui.h"
#include "External Libs/imgui/imgui_impl_glfw.h"
#include "External Libs/imgui/imgui_impl_opengl3.h"

#include "Window.h"


UIManager::UIManager(const Window& window)
{
	OnInit(window);
}

void UIManager::OnInit(const class Window& window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	m_ImGuiWantCaptureMouse = &io.WantCaptureMouse;
	ImGui_ImplGlfw_InitForOpenGL(window.GetNativeWindow(), true);
	//ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 400"); //or version 430


	//Fonts
	//emplace_back does not create a copy of element before adding to the list
	//m_Fonts.emplace_back(std::make_unique<ImFont>(io.Fonts->AddFontDefault()));
	m_Fonts.emplace(m_AvaliableFonts.emplace_back("default"), io.Fonts->AddFontDefault());
	//will not work be a copy is created with push_back
	//m_Fonts.push_back(std::make_unique<ImFont>(def_font));  

	m_Fonts.emplace(m_AvaliableFonts.emplace_back("Roboto-Bold"), io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto/Roboto-Bold.ttf", 35.0f));
	m_Fonts.emplace(m_AvaliableFonts.emplace_back("Roboto-Light"), io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto/Roboto-Light.ttf", 15.0f));
	m_Fonts.emplace(m_AvaliableFonts.emplace_back("OpenSans-Bold"), io.Fonts->AddFontFromFileTTF("Assets/Fonts/Open_Sans/OpenSans-Bold.ttf", 15.0f));
	m_Fonts.emplace(m_AvaliableFonts.emplace_back("OpenSans-Regular"), io.Fonts->AddFontFromFileTTF("Assets/Fonts/Open_Sans/OpenSans-Regular.ttf", 15.0f));
	m_Fonts.emplace(m_AvaliableFonts.emplace_back("OpenSans-Light"), io.Fonts->AddFontFromFileTTF("Assets/Fonts/Open_Sans/OpenSans-Light.ttf", 15.0f));



	//m_Fonts.push_back(std::make_unique<ImFont>(io.Fonts->AddFontFromFileTTF("Assets/Fonts/", 10.0f)));
	//m_Fonts.push_back(io.Fonts->AddFontDefault());
}

void UIManager::OnStartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UIManager::OnEndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::OnDestroy()
{
	//Clean fonts

	//TimeTaken ShuttingDown("Shutting down program");
	m_ImGuiWantCaptureMouse = nullptr;     //Didnt use the new keyword so its fine 
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


ImFont* UIManager::GetFont(std::string font_name)
{
	auto it = m_Fonts.find(font_name);

	if (it != m_Fonts.end());
	{
		return it->second;
	}

	printf("Font '%s' does not exist or is not registered!!!\n", font_name);
	return ImGui::GetFont();
}

UIManager::~UIManager()
{
	//fix late
	//OnDestroy();

}
