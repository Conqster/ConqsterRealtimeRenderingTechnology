#pragma once
#include <iostream>
#include "Game.h"

//Experimental
#include "External Libs/imgui/imgui.h"

int main()
{
	Game* new_game = new Game();

	new_game->OnStart();

	new_game->SetMenubarCallback([new_game]()
	{

		static bool loadSceneWin = false;
		if (loadSceneWin)  new_game->OnLoadSceneUI("Scenes", &loadSceneWin);

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Load Scene"))
			{
				ImGui::MenuItem("OpenScenes", nullptr, &loadSceneWin);

				//new_game->OnLoadSceneUI("Scenes"); //Rename Scenes
				//List scene
				if(ImGui::MenuItem("Temp Scene1"))
				{ }
				if(ImGui::MenuItem("Temp Scene2"))
				{ }
				if(ImGui::MenuItem("Temp Scene3"))
				{ }
				if(ImGui::MenuItem("Temp Scene4"))
				{ }

				ImGui::EndMenu();
			}

			ImGui::SeparatorText("Experimental ~Not functional");
			if (ImGui::MenuItem("Save", "Ctrl + S"))
			{

			}
			if (ImGui::MenuItem("Save As...", "Ctrl + Shift + S"))
			{

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
			{
				new_game->CloseProgram();
			}
			ImGui::EndMenu();
		}




		//----------------------------------------------Edit-------------------------------------------------------------------
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::Separator();
			//if(ImGui::IsItemClicked())
			if (ImGui::BeginMenu("Change Font"))
			{
				new_game->ChangeUIFont();
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		//----------------------------------------------Stat-------------------------------------------------------------------
		if (ImGui::BeginMenu("Stat"))
		{
			if (ImGui::BeginMenu("All Stats"))
			{
				new_game->AllStatsTab();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("General Stats"))
			{
				new_game->StatsUI();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Camera Properties"))
			{
				//load camera Properties
				new_game->CameraStatsUI();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window Properties"))
			{
				new_game->WindowStatsUI();
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}


		//-------------------------------------------------Help-------------------------------------------------------------------
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Controls"))
			{

			}
			if (ImGui::MenuItem("About"))
			{

			}
			ImGui::EndMenu();
		}
	});

	new_game->Run();

	//std::cin.get();
	return 0;
}