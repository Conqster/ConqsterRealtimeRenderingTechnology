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
	ImGui_ImplGlfw_InitForOpenGL(window.GetNativeWindow(), true);
	//ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 400"); //or version 430
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
	//TimeTaken ShuttingDown("Shutting down program");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

UIManager::~UIManager()
{
	//fix late
	//OnDestroy();
}
