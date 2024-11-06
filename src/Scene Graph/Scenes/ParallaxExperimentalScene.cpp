#include "ParallaxExperimentalScene.h"

#include "External Libs/imgui/imgui.h"
#include "Util/FilePaths.h"
#include "Util/MathsHelpers.h"

#include <glm/gtx/quaternion.hpp>

#include "../Entity.h"
#include "Renderer/RenderCommand.h"

void ParallaxExperimentalScene::SetWindow(Window* window)
{
	this->window = window;
}

void ParallaxExperimentalScene::OnInit(Window* window)
{
	Scene::OnInit(window);

	window->UpdateProgramTitle("Parallax Experimental Scene");


	RenderCommand::EnableDepthTest();
	RenderCommand::EnableFaceCull();
	RenderCommand::CullBack();



	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 1.0f/*0.5f*/);

	CreateObjects();
}

void ParallaxExperimentalScene::OnUpdate(float delta_time)
{
	////////////////////
	// update light matrix  data
	////////////////////
	//dirLightObject.sampleWorldPos = playerTest.aabb.GetCenter() + glm::vec3(0.0f, 0.0f, 1.0) * playerTest.shadowOffset;
	//if (dirLightObject.dirlight.castShadow)
	if (enableSceneShadow)
	{
		dirLightObject.dirLightShadow.UpdateProjMat();
		dirLightObject.dirLightShadow.UpdateViewMatrix(dirLightObject.sampleWorldPos, 
													   dirLightObject.dirlight.direction,
													   dirLightObject.cam_offset);
	}



	//point shadow far update 
	float shfar = ptShadowConfig.cam_far;
	ptLight[0].shadow_far = shfar;
	//for (auto& pt : lightObject)


	OnRender();
}

void ParallaxExperimentalScene::OnRender()
{
	RenderCommand::ClearColour(m_ClearScreenColour);
	RenderCommand::Clear();

	////////////////////////////////////////
	// UPDATE UNIFORM BUFFERs
	////////////////////////////////////////
	//------------------Camera Matrix Data UBO-----------------------------/
	m_CamMatUBO.SetSubDataByID(&(m_Camera->CalculateProjMatrix(window->GetAspectRatio())[0][0]), sizeof(glm::mat4), 0);
	m_CamMatUBO.SetSubDataByID(&(m_Camera->CalViewMat()[0][0]), sizeof(glm::mat4), sizeof(glm::mat4));


	LightPass(); //Update Light data buffer
	if(enableSceneShadow)
		ShadowPass();

	
	/////////////////////
	// First Pass : Draw Scene to HDR buffer
	/////////////////////#
	hdrFBO.Bind();
	RenderCommand::Clear();
	//for shadow
	modelShader.Bind();

	//would be good to have this in camera buffer
	modelShader.SetUniformVec3("u_ViewPos", m_Camera->GetPosition());

	modelShader.SetUniform1i("u_EnableSceneShadow", enableSceneShadow);
	if (enableSceneShadow)
	{
		modelShader.SetUniformMat4f("u_DirLightSpaceMatrix", dirLightObject.dirLightShadow.GetLightSpaceMatrix());
		//tex unit 0 >> texture 
		//tex unit 1 >> potenially normal map
		//tex unit 2 >> potenially parallax map
		//tex unit 3 >> shadow map (dir Light)
		//tex unit 4 >> shadow cube (pt Light)
		dirDepthMap.Read(3);
		modelShader.SetUniform1i("u_DirShadowMap", 3);
		ptDepthCube.Read(4);
		modelShader.SetUniform1i("u_PointShadowCube", 4);
	}
	DrawObjects(modelShader, true);
	glowingCubeShader.Bind();
	glowingCube.Render();
	glowingCubeShader.SetUniformMat4f("u_Model", glowingCubeTrans);
	glowingCubeShader.UnBind();
	hdrFBO.UnBind();

	/////////////////////
	// Second Pass : Depth Test
	/////////////////////
	depthFBO.Bind();
	RenderCommand::Clear();
	DrawObjects(depthShader, false);
	glowingCubeShader.Bind();
	glowingCube.Render(); //model pos already set 
	glowingCubeShader.UnBind();
	depthFBO.UnBind();


	////////////////////////
	// TEST PASS : Multi Render Pass 
	////////////////////////
	MRT_FBO.Bind();
	RenderCommand::ClearColour(m_ClearScreenColour);
	RenderCommand::Clear();
	MRTShader.Bind();
	floorMat->baseMap->DisActivate();
	MRTShader.SetUniformVec3("u_ViewPos", m_Camera->GetPosition());
	MRTShader.SetUniform1i("u_EnableSceneShadow", enableSceneShadow);
	MRTShader.SetUniform1f("rate", bloomrate);

	if (enableSceneShadow)
	{
		MRTShader.SetUniformMat4f("u_DirLightSpaceMatrix", dirLightObject.dirLightShadow.GetLightSpaceMatrix());
		//tex unit 0 >> texture 
		//tex unit 1 >> potenially normal map
		//tex unit 2 >> potenially parallax map
		//tex unit 3 >> shadow map (dir Light)
		//tex unit 4 >> shadow cube (pt Light)
		dirDepthMap.Read(3);
		MRTShader.SetUniform1i("u_DirShadowMap", 3);
		ptDepthCube.Read(4);
		MRTShader.SetUniform1i("u_PointShadowCube", 4);
	}
	DrawObjects(MRTShader, true);
	MRTShader.UnBind();
	glowingCubeShader.Bind();
	glowingCubeShader.SetUniform1f("rate", bloomrate);
	glowingCubeShader.SetUniformVec3("u_Colour", glowingCubeColour);
	glowingCube.Render(); //model pos already set 
	glowingCubeShader.UnBind();
	MRT_FBO.UnBind();



	/////////////////////
	// Third Pass : Draw HDR buffer image
	/////////////////////
	//hdrPostShader.Bind();
	////hdrFBO.BindTexture(0);
	//MRT_FBO.BindTextureIdx(1, 0);
	//hdrPostShader.SetUniform1f("exposure", hdrExposure);
	//GLCall(glBindVertexArray(hdrPostProcessQuad.VAO));
	//GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	//GLCall(glBindVertexArray(0));
	//hdrPostShader.UnBind();

	//use bloom 
	bloomShader.Bind();
	MRT_FBO.BindTextureIdx(1, 0);
	bloomShader.SetUniform1i("horizontal", horiBloom);
	bloomShader.SetUniform1f("rate", 1.0f);
	GLCall(glBindVertexArray(hdrPostProcessQuad.VAO));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	GLCall(glBindVertexArray(0));
	bloomShader.UnBind();


	//return;
	//Render scene on horizontal then vertical 
	bool horizontal = true;
	bool first_it = true;
	int amount = 10;
	//GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	for (int i = 0; i < amount; i++)
	{
		if (horizontal)
			pingFBO2.Bind();
		else
			pingFBO1.Bind();
		bloomShader.Bind();
		RenderCommand::Clear();
		//set shader to horizontal 
		bloomShader.SetUniform1i("horizontal", horizontal);
		bloomShader.SetUniform1f("rate", 1.0f);

		if(first_it)
			MRT_FBO.BindTextureIdx(1, 0); //use the actual prev bloom output 
		else
		{
			if (horizontal)
				pingFBO1.BindTexture(0);
			else
				pingFBO2.BindTexture(0);
		}
		//draw screen quad
		GLCall(glBindVertexArray(hdrPostProcessQuad.VAO));
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
		GLCall(glBindVertexArray(0));

		horizontal = !horizontal;
		if (first_it)
			first_it = false;
	}
	pingFBO1.UnBind();

	//pingFBO2.Bind();
	//bloomShader.Bind();
	//GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	//MRT_FBO.BindTextureIdx(1, 0);
	//bloomShader.SetUniform1i("horizontal", horiBloom);
	//bloomShader.SetUniform1f("rate", 1.0f);
	//GLCall(glBindVertexArray(hdrPostProcessQuad.VAO));
	//GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	//GLCall(glBindVertexArray(0));
	//bloomShader.UnBind();
	//pingFBO2.UnBind();


	//use final bloom
	RenderCommand::Clear();
	pingBloomShader.Bind();
	hdrFBO.BindTexture(0);
	pingBloomShader.SetUniform1i("u_SceneTex", 0);
	if (horizontal)
		pingFBO1.BindTexture(1);
	else
		pingFBO2.BindTexture(1);
	pingBloomShader.SetUniform1i("u_BloomBlurTex", 1);
	pingBloomShader.SetUniform1f("u_Exposure", hdrExposure);
	GLCall(glBindVertexArray(hdrPostProcessQuad.VAO));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	GLCall(glBindVertexArray(0));
	pingBloomShader.UnBind();

	return;

	//Debuging 
	if (ptLightGizmos)
	{
		for (int i = 0; i < MAX_POINT_LIGHT; i++)
		{
			DebugGizmos::DrawWireThreeDisc(ptLight[i].position, 2.0f, 10, ptLight[i].colour, 1.0f);
			DebugGizmos::DrawCross(ptLight[i].position);
		}
	}


	//test directional Shadow info 
	if (dirLightObject.dirLightShadow.debugPara)
	{
		auto& ds = dirLightObject.dirLightShadow;
		float dcv = dirLightObject.cam_offset + ds.config.cam_near * 0.5f; //dcv is the center/value between the near & far plane 
		glm::vec3 orthCamPos = dirLightObject.sampleWorldPos + (dirLightObject.dirlight.direction * dirLightObject.cam_offset);
		glm::vec3 farPlane = orthCamPos + (glm::normalize(-dirLightObject.dirlight.direction) * ds.config.cam_far);
		glm::vec3 nearPlane = orthCamPos + (glm::normalize(dirLightObject.dirlight.direction) * ds.config.cam_near);
		DebugGizmos::DrawOrthoCameraFrustrm(orthCamPos, dirLightObject.dirlight.direction,
			ds.config.cam_near, ds.config.cam_far, ds.config.cam_size,
			glm::vec3(0.0f, 1.0f, 0.0f));

		//Shadow Camera Sample Position 
		DebugGizmos::DrawCross(dirLightObject.sampleWorldPos);
	}

}

void ParallaxExperimentalScene::OnRenderUI()
{
	ImGui::Begin("Parallax Experimental Scene");

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

	////////////////////////////////////////////////
	// SCENE OBJECTS
	////////////////////////////////////////////////
	ImGui::Spacing();
	if (ImGui::TreeNode("Scene Objects"))
	{
		ImGui::Checkbox("Use Normal Map", &useNor);
		glm::vec3 translate, euler, scale;



		ImGui::PushID("Ground");
		MathsHelper::DecomposeTransform(groundWorldTrans, translate, euler, scale);
		bool update = ImGui::DragFloat3("Translate", &translate[0], 0.2f);
		update |= ImGui::DragFloat3("Rotation", &euler[0], 0.2f);
		update |= ImGui::DragFloat3("Scale", &scale[0], 0.2f);
		if (update)
		{
			glm::quat quat = glm::quat(glm::radians(euler));
			glm::mat4 rot = glm::toMat4(quat);
			groundWorldTrans = glm::translate(glm::mat4(1.0f), translate) * rot * glm::scale(glm::mat4(1.0f), scale);
		}
		ImGui::PopID();

		ImGui::SeparatorText("Glowing Cube");
		ImGui::PushID("Glowing Cube");
		ImGui::ColorEdit3("Glow Colour", &glowingCubeColour[0]);
		MathsHelper::DecomposeTransform(glowingCubeTrans, translate, euler, scale);
		update = ImGui::DragFloat3("Translate", &translate[0], 0.2f);
		update |= ImGui::DragFloat3("Rotation", &euler[0], 0.2f);
		update |= ImGui::DragFloat3("Scale", &scale[0], 0.2f);
		if (update)
			glowingCubeTrans = glm::translate(glm::mat4(1.0f), translate) *
			glm::toMat4(glm::quat(glm::radians(euler))) *
			glm::scale(glm::mat4(1.0f), scale);
		ImGui::PopID();

		ImGui::SeparatorText("Plane");
		ImGui::PushID("Plane");
		MathsHelper::DecomposeTransform(planeWorldTran, translate, euler, scale);
		update = ImGui::DragFloat3("Translate", &translate[0], 0.2f);
		update |= ImGui::DragFloat3("Rotation", &euler[0], 0.2f);
		update |= ImGui::DragFloat3("Scale", &scale[0], 0.2f);
		if (update)
			planeWorldTran = glm::translate(glm::mat4(1.0f), translate) * 
							 glm::toMat4(glm::quat(glm::radians(euler))) * 
							 glm::scale(glm::mat4(1.0f), scale);
		ImGui::PopID();

		ImGui::SeparatorText("Blender Shapes");
		ImGui::PushID("Blender Shapes");
		//euler = scale = glm::vec3(0.0f);
		MathsHelper::DecomposeTransform(shapesTrans, translate, euler, scale);
		update = ImGui::DragFloat3("Translate", &translate[0], 0.2f);
		update |= ImGui::DragFloat3("Rotation", &euler[0], 0.2f);
		update |= ImGui::DragFloat3("Scale", &scale[0], 0.2f);
		if (update)
			shapesTrans = glm::translate(glm::mat4(1.0f), translate) *
						  glm::toMat4(glm::quat(glm::radians(euler))) *
						  glm::scale(glm::mat4(1.0f), scale);
		ImGui::PopID();

		ImGui::TreePop();
	}

	///////////////////////////////////////////
	// SCENE LIGHTS
	///////////////////////////////////////////
	ImGui::Spacing();
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::SeparatorText("Light Global Properties");
		ImGui::Checkbox("Enable Scene Shadow", &enableSceneShadow);
		ImGui::SliderFloat("HDR Exposure", &hdrExposure, 0.0f, 1.0f);
		ImGui::Checkbox("Horizontal Bloom", &horiBloom);
		ImGui::SliderFloat("Bloom Rate", &bloomrate , 0.0f, 10.0f);


		//////////////////////////////////////
		// Directional Light
		//////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Directional Light");
		ImGui::Checkbox("Enable Directional", &dirLightObject.dirlight.enable);
		//ImGui::SameLine();
		//ImGui::Checkbox("Cast Shadow", &dirLightObject.dirlight.castShadow);
		ImGui::DragFloat3("Light Direction", &dirLightObject.dirlight.direction[0], 0.1f, -1.0f, 1.0f);
		ImGui::ColorEdit3("Dir Light colour", &dirLightObject.dirlight.colour[0]);
		ImGui::SliderFloat("Light ambinentIntensity", &dirLightObject.dirlight.ambientIntensity, 0.0f, 1.0f);
		ImGui::SliderFloat("Light diffuseIntensity", &dirLightObject.dirlight.diffuseIntensity, 0.0f, 1.0f);
		ImGui::SliderFloat("Light specIntensity", &dirLightObject.dirlight.specularIntensity, 0.0f, 1.0f);
		if (dirLightObject.dirlight.castShadow)
		{
			if (ImGui::TreeNode("Shadow Camera Info"))
			{
				auto& shadow = dirLightObject.dirLightShadow;
				ImGui::SliderFloat("Camera Near", &shadow.config.cam_near, 0.0f, shadow.config.cam_far - 0.5f);
				ImGui::SliderFloat("Camera Far", &shadow.config.cam_far, shadow.config.cam_near + 0.5f, 1000.0f);
				ImGui::SliderFloat("Camera Size", &shadow.config.cam_size, 0.0f, 200.0f);
				ImGui::DragFloat3("Sample Pos", &dirLightObject.sampleWorldPos[0], 0.1f);
				ImGui::SliderFloat("Light Proj Offset", &dirLightObject.cam_offset, 0.0f, 100.0f);
				ImGui::Checkbox("Debug Dir Shadow Para", &shadow.debugPara);

				ImGui::TreePop();
			}
		}
		ImGui::Spacing();


		//////////////////////////////////////////
		// Point Lights
		//////////////////////////////////////////
		ImGui::SeparatorText("Point Lights");
		if (ImGui::TreeNode("Points Lights"))
		{
			ImGui::Checkbox("Point Light Gizmos", &ptLightGizmos);
			if (ImGui::TreeNode("Shadow Camera Info"))
			{
				auto& shadow = ptShadowConfig;
				ImGui::Checkbox("Debug Pt Lights", &shadow.debugLight);
				ImGui::SliderFloat("Pt Shadow Camera Near", &shadow.cam_near, 0.0f, shadow.cam_far - 0.5f);
				ImGui::SliderFloat("Pt Shadow Camera Far", &shadow.cam_far, shadow.cam_near + 0.5f, 80.0f);
				ImGui::TreePop();
			}


			for(int i = 0; i < MAX_POINT_LIGHT; i++)
			{
				std::string label = "point light: " + std::to_string(i);
				ImGui::SeparatorText(label.c_str());
				ImGui::Checkbox((label + " Enable light").c_str(), &ptLight[i].enable);
				//ImGui::SameLine();
				//ImGui::Checkbox((label + "cast shadow").c_str(), &ptLight.castShadow);

				ImGui::DragFloat3((label + " position").c_str(), &ptLight[i].position[0], 0.1f);

				ImGui::ColorEdit3((label + " colour").c_str(), &ptLight[i].colour[0]);
				ImGui::SliderFloat((label + " ambinentIntensity").c_str(), &ptLight[i].ambientIntensity, 0.0f, 1.0f);
				ImGui::SliderFloat((label + " diffuseIntensity").c_str(), &ptLight[i].diffuseIntensity, 0.0f, 1.0f);
				ImGui::SliderFloat((label + " specIntensity").c_str(), &ptLight[i].specularIntensity, 0.0f, 1.0f);
				ImGui::SliderFloat((label + " constant attenuation").c_str(), &ptLight[i].attenuation[0], 0.0f, 1.0f);
				ImGui::SliderFloat((label + " linear attenuation").c_str(), &ptLight[i].attenuation[1], 0.0f, 1.0f);
				ImGui::SliderFloat((label + " quadratic attenuation").c_str(), &ptLight[i].attenuation[2], 0.0f, 1.0f);
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	ImGui::End();

	if (sceneEntities.size() > 0)
	{
		ImGui::Begin("Entities Debug UI");
		for (auto& e : sceneEntities)
			EntityDebugUI(*e);
		ImGui::End();
	}


	

	ImGui::Begin("Frame Buffers");
	ImVec2 glob_size(400.0f, 400.0f);
	glob_size.y *= (hdrFBO.GetSize().y / hdrFBO.GetSize().x); //invert
	ImGui::SeparatorText("Rendered Scene");
	ImGui::Image((ImTextureID)(intptr_t)hdrFBO.GetColourAttachment(), glob_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	ImGui::SeparatorText("Scene Frame View Depth");
	///glob_size = ImVec2(400.0f, 400.0f);
	//glob_size.y *= (depthFBO.GetSize().y / depthFBO.GetSize().x); //invert
	ImGui::Image((ImTextureID)(intptr_t)depthFBO.GetColourAttachment(), glob_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	ImGui::Spacing();
	ImGui::SeparatorText("Scene Frag");
	//glob_size = ImVec2(550.0f, 550.0f);
	//glob_size.y *= (MRT_FBO.GetSize().y / MRT_FBO.GetSize().x); //invert
	ImGui::Image((ImTextureID)(intptr_t)MRT_FBO.GetColourAttachment(0), glob_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	ImGui::SeparatorText("Scene Bright Frag");
	ImGui::Image((ImTextureID)(intptr_t)MRT_FBO.GetColourAttachment(1), glob_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	ImGui::SeparatorText("Scene Solid/Albedo Frag");
	ImGui::Image((ImTextureID)(intptr_t)MRT_FBO.GetColourAttachment(2), glob_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	ImGui::Spacing();
	ImGui::SeparatorText("Scene Horizontal Blur");
	ImGui::Image((ImTextureID)(intptr_t)pingFBO1.GetColourAttachment(), glob_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	ImGui::SeparatorText("Scene Vertical Blur");
	ImGui::Image((ImTextureID)(intptr_t)pingFBO2.GetColourAttachment(), glob_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	ImGui::End();

	bool mat_ui = ImGui::Begin("Material");
	if (mat_ui)
	{
		int tex_id;
		static int blank_tex_id = blank_tex->GetID();
		auto temp_mat = floorMat;
		ImGui::PushID(temp_mat->name);
		ImGui::Text("1. %s", temp_mat->name);
		ImGui::ColorEdit3("Colour", &temp_mat->baseColour[0]);
		tex_id = (temp_mat->baseMap) ? temp_mat->baseMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Main Texture");
		tex_id = (temp_mat->normalMap) ? temp_mat->normalMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Normal Map");
		tex_id = (temp_mat->parallaxMap) ? temp_mat->parallaxMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Parallax/Height Map");
		ImGui::Checkbox("Use Parallax", &temp_mat->useParallax);
		ImGui::SliderFloat("Parallax/Height Scale", &temp_mat->heightScale, 0.0f, 0.08f);
		ImGui::SliderInt("Shinness", &temp_mat->shinness, 32, 256);
		ImGui::PopID();
		floorMat = temp_mat;

		temp_mat = planeMat;
		ImGui::PushID(temp_mat->name);
		ImGui::Text("2. %s", temp_mat->name);
		ImGui::ColorEdit3("Colour", &temp_mat->baseColour[0]);
		tex_id = (temp_mat->baseMap) ? temp_mat->baseMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Main Texture");
		tex_id = (temp_mat->normalMap) ? temp_mat->normalMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Normal Map");
		tex_id = (temp_mat->parallaxMap) ? temp_mat->parallaxMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Parallax/Height Map");
		ImGui::Checkbox("Use Parallax", &temp_mat->useParallax);
		ImGui::SliderFloat("Parallax/Height Scale", &temp_mat->heightScale, 0.0f, 0.08f);
		ImGui::SliderInt("Shinness", &temp_mat->shinness, 32, 256);
		ImGui::PopID();
		planeMat = temp_mat;

		temp_mat = wallMat;
		ImGui::PushID(temp_mat->name);
		ImGui::Text("3. %s", temp_mat->name);
		ImGui::ColorEdit3("Colour", &temp_mat->baseColour[0]);
		tex_id = (temp_mat->baseMap) ? temp_mat->baseMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Main Texture");
		tex_id = (temp_mat->normalMap) ? temp_mat->normalMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Normal Map");
		tex_id = (temp_mat->parallaxMap) ? temp_mat->parallaxMap->GetID() : blank_tex_id;
		ImGui::Image((ImTextureID)(intptr_t)tex_id, ImVec2(100, 100));
		ImGui::SameLine(); ImGui::Text("Parallax/Height Map");
		ImGui::Checkbox("Use Parallax", &temp_mat->useParallax);
		ImGui::SliderFloat("Parallax/Height Scale", &temp_mat->heightScale, 0.0f, 0.08f);
		ImGui::SliderInt("Shinness", &temp_mat->shinness, 32, 256);
		ImGui::PopID();
		wallMat = temp_mat;
	}
	ImGui::End();

}

void ParallaxExperimentalScene::OnDestroy()
{
	//if (brickTex)
	//	brickTex->Clear();

	//if (brickNorMap)
	//	brickNorMap->Clear();

	if (floorMat->baseMap)
		floorMat->baseMap->Clear();
	if (floorMat->normalMap)
		floorMat->normalMap->Clear();
	if (floorMat->parallaxMap)
		floorMat->parallaxMap->Clear();

	if (planeMat->baseMap)
		planeMat->baseMap->Clear();
	if (planeMat->normalMap)
		planeMat->normalMap->Clear();
	if (planeMat->parallaxMap)
		planeMat->parallaxMap->Clear();


	GLCall(glDeleteBuffers(1, &hdrPostProcessQuad.VBO));
	GLCall(glDeleteVertexArrays(1, &hdrPostProcessQuad.VAO));


	Scene::OnDestroy();
}

void ParallaxExperimentalScene::CreateObjects()
{
	ground.Create();
	glowingCube.Create();
	ShaderFilePath shader
	{
		"Assets/Shaders/Learning/ParallaxExperiment/ParallaxModelVertex.glsl", //vertex shader
		"Assets/Shaders/Learning/GlowingObjectFrag.glsl", //fragment shader
	};
	glowingCubeShader.Create("mrt_shader", shader);
	glowingCubeTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 2.0f, 0.0f));

	//Post process quad
	float vertices[] 
	{
		/*0*/ -1.0f, 1.0f,  0.0f, 1.0f,
		/*1*/ -1.0f, -1.0f, 0.0f, 0.0f,
		/*2*/ 1.0f, -1.0f,	1.0f, 0.0f,

		/*3*/ -1.0f, 1.0f,	0.0f, 1.0f,
		/*2*/ 1.0f, -1.0f,	1.0f, 0.0f,
		/*2*/ 1.0f, 1.0f,	1.0f, 1.0f,
	};


	GLCall(glGenVertexArrays(1, &hdrPostProcessQuad.VAO));
	GLCall(glGenBuffers(1, &hdrPostProcessQuad.VBO));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, hdrPostProcessQuad.VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLCall(glBindVertexArray(hdrPostProcessQuad.VAO));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 4, (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 4, (void*)(sizeof(vertices[0]) * 2)));

	//Post Process frame buffer 
	hdrFBO.Generate(window->GetWidth(), window->GetHeight(), FBO_Format::RGBA16F);
	//Post Process shader
	ShaderFilePath screen_shader_file_path
	{
		"Assets/Shaders/Learning/MSAA/AA_PostProcess_Vertex.glsl",
		"Assets/Shaders/Learning/Post Process/PostProcessHDRfrag.glsl"
	};
	hdrPostShader.Create("HDR post shader", screen_shader_file_path);

	depthFBO.Generate(window->GetWidth(), window->GetHeight());
	ShaderFilePath depth_test_shader
	{
		"Assets/Shaders/Learning/Depth/SceneDepthViewVertex.glsl",
		"Assets/Shaders/Learning/Depth/SceneDepthViewFrag.glsl"
	};
	depthShader.Create("depth test shader", depth_test_shader);


	//Multiple Render Target
	MRT_FBO.Generate(window->GetWidth(), window->GetHeight(), FBO_Format::RGBA16F);
	ShaderFilePath mrt_shader
	{
		"Assets/Shaders/Learning/ParallaxExperiment/ParallaxModelVertex.glsl", //vertex shader
		"Assets/Shaders/Learning/MultiColourAttachment/SceneCol_Bright_Solid.glsl", //fragment shader
	};
	MRTShader.Create("mrt_shader", mrt_shader);

	//effect
	//Post Process shader
	ShaderFilePath bloom_shader_file_path
	{
		"Assets/Shaders/Learning/MSAA/AA_PostProcess_Vertex.glsl",
		"Assets/Shaders/Learning/Effects/SimpleGaussianBlurBloom.glsl"
	};
	bloomShader.Create("bloom effect post shader", bloom_shader_file_path);

	pingFBO1.Generate(window->GetWidth(), window->GetHeight(), FBO_Format::RGBA16F);
	pingFBO2.Generate(window->GetWidth(), window->GetHeight(), FBO_Format::RGBA16F);
	ShaderFilePath ping_bloom_shader_file_path
	{
		"Assets/Shaders/Learning/MSAA/AA_PostProcess_Vertex.glsl",
		"Assets/Shaders/Learning/Effects/TestBloomResult.glsl"
	};
	pingBloomShader.Create("bloom effect post shader", ping_bloom_shader_file_path);
	//////////////////////////////////////
	// GENERATE SHADERS
	//////////////////////////////////////
	//model shader
	ShaderFilePath shader_file_path
	{
		"Assets/Shaders/Learning/ParallaxExperiment/ParallaxModelVertex.glsl", //vertex shader
		"Assets/Shaders/Learning/ParallaxExperiment/ParallaxModelFrag.glsl", //fragment shader
	};
	modelShader.Create("model_shader", shader_file_path);
	//depth test shader
	ShaderFilePath point_shadow_shader_file_path
	{
		"Assets/Shaders/ShadowMapping/ShadowDepthVertex.glsl", //vertex shader
		"Assets/Shaders/ShadowMapping/ShadowDepthFrag.glsl", //fragment shader
		"Assets/Shaders/ShadowMapping/ShadowDepthGeometry.glsl", //geometry shader
	};

	shadowDepthShader.Create("point_shadow_depth", point_shadow_shader_file_path);


	////////////////////////////////////////
	// CREATE TEXTURES 
	////////////////////////////////////////
	floorMat = std::make_shared<Material>();
	floorMat->name = "Floor Mat";
	floorMat->baseMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("cobblestone-diff"));
	floorMat->normalMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("cobblestone-nor"));
	floorMat->parallaxMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("cobblestone-disp"));

	planeMat = std::make_shared<Material>();
	planeMat->name = "Plane Material";
	planeMat->baseMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("plain"));

	wallMat = std::make_shared<Material>();
	wallMat->name = "Wall Material";
	wallMat->baseMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("old_plank"));

	//rotate plane 180 around  y 
	///glm::mat4 model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	groundWorldTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -120.0f, 0.0f)) *
					   glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
					   glm::scale(glm::mat4(1.0f), glm::vec3(50.0f));

	planeWorldTran = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, 20.0f, 5.0f)) *
					 glm::rotate(planeWorldTran, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
					 glm::rotate(planeWorldTran, glm::radians(-50.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
					 glm::scale(planeWorldTran, glm::vec3(20.0f));	

	
	blenderShapes = modelLoader.Load(FilePaths::Instance().GetPath("shapes"), true);
	shapesTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -200.0f, 25.0f)) *
					 glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
					 glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

		
	////////////////////////////////////////
	// UNIFORM BUFFERs
	////////////////////////////////////////
	//------------------Camera Matrix Data UBO-----------------------------/
	long long int buf_size = 2 * sizeof(glm::mat4);// +sizeof(glm::vec2);   //to store view, projection
	m_CamMatUBO.Generate(buf_size);
	m_CamMatUBO.BindBufferRndIdx(0, buf_size, 0);
	
	//i think what happens here is that the shader program 
	//scan/check its itself(program) for the block "u_CameraMat"
	//woudl be success if found

	//------------------Light Data UBO-----------------------------/
	//struct
	//DirectionalLight dirLight;
	//PointLight pointLights[MAX_POINT_LIGHTS];
	long long int light_buffer_size = 0;
	light_buffer_size += DirectionalLight::GetGPUSize();		//get the size of a directional light
	//int pt_light_count = 1;
	light_buffer_size += MAX_POINT_LIGHT/*pt_light_count*/ * PointLight::GetGPUSize();				//get the size of a point light

	m_LightDataUBO.Generate(light_buffer_size);
	m_LightDataUBO.BindBufferRndIdx(1, light_buffer_size, 0);
	

	//Assign UBO, if necessary 
	modelShader.Bind();
	modelShader.SetUniformBlockIdx("u_CameraMat", 0);
	modelShader.SetUniformBlockIdx("u_LightBuffer", 1);
	MRTShader.Bind();
	MRTShader.SetUniformBlockIdx("u_CameraMat", 0);
	MRTShader.SetUniformBlockIdx("u_LightBuffer", 1);
	
	glowingCubeShader.Bind();
	glowingCubeShader.SetUniformBlockIdx("u_CameraMat", 0);
	

	//---------------------Test Entity------------------------------/
	//std::vector<Entity> sceneEntities;
	//create a plane entity 
	glm::mat4 temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 0.0f)) *
						   glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
						   glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	int id_idx = 0;
	Entity floor_plane_entity = Entity(id_idx++, "floor-plane-entity", temp_trans, std::make_shared<Mesh>(ground), floorMat);
	sceneEntities.emplace_back(std::make_shared<Entity>(floor_plane_entity));
	Entity plane2_entity = Entity(id_idx++, "plane-entity", temp_trans, std::make_shared<Mesh>(ground), planeMat);
	sceneEntities.emplace_back(std::make_shared<Entity>(plane2_entity));
	Entity wall_plane_entity = Entity(id_idx++, "wall-plane-entity", temp_trans, std::make_shared<Mesh>(ground), wallMat);
	sceneEntities.emplace_back(std::make_shared<Entity>(wall_plane_entity));
	Entity cube_entity = Entity(id_idx++, "cube-entity", temp_trans, std::make_shared<Mesh>(glowingCube), planeMat);
	sceneEntities.emplace_back(std::make_shared<Entity>(cube_entity));

	temp_trans = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));/* *
				 glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
				 glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));*/
	
	Entity cube2_entity = Entity(id_idx++, "cube2-entity", temp_trans, std::make_shared<Mesh>(glowingCube), planeMat);
	//cube2_entity.SetParent(std::make_shared<Entity>(sceneEntities.back()));
	sceneEntities.back()->AddLocalChild(cube2_entity);
	Entity cube3_entity = Entity(id_idx++, "cube2-entity", temp_trans, std::make_shared<Mesh>(glowingCube), wallMat);
	sceneEntities.back()->AddWorldChild(cube3_entity);


	//--------------------Light--------------------------------/
	//Dir light
	auto& dl = dirLightObject.dirlight;
	dirLightObject.sampleWorldPos = glm::vec3(5.0f, 2.0f, -10.0f);
	dirLightObject.cam_offset = 10.0f;
	dl.ambientIntensity = 0.05f;
	dl.diffuseIntensity = 0.4f;
	dl.specularIntensity = 0.2f;
	dl.colour = glm::vec3(1.0f, 0.9568f, 0.8392f);
	dl.enable = true;
	dl.castShadow = true;
	dl.direction = glm::vec3(-1.0f, 1.0f, -1.0f);

	//Point Light
	//ptLight.position = glm::vec3(0.0f, 5.0f, 0.0f);

	for (int i = 0; i < MAX_POINT_LIGHT; i++)
	{
		ptLight[i].colour = glm::vec3(20.0f);
		ptLight[i].ambientIntensity = 0.5f; // 1.0f;// 0.05f;
		ptLight[i].diffuseIntensity = 0.4f;
		ptLight[i].specularIntensity = 0.6f;
		ptLight[i].enable = true;
		ptLight[i].attenuation[0] = 1.0f; //constant
		ptLight[i].attenuation[1] = 0.07f; //linear
		ptLight[i].attenuation[2] = 0.017f; //quadratic

		if (i == 4 || i == 5 || i == 6)
		{
			if (i == 4)
			{
				ptLight[i].colour *= glm::vec3(1.0f, 0.0f, 0.0f);
				ptLight[i].position = glm::vec3(-4.0f, -0.2f, -9.0f);
			}
			else
			{
				ptLight[i].colour *= (i == 5) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 1.0f);
				ptLight[i].position = (i == 5) ? glm::vec3(7.0f, -0.2f, -20.0f) : glm::vec3(0.0f, -0.2f, -34.0f);
			}
		}
	}

	//define pt light pos
	glm::vec3 p = glm::vec3(9.0f, 17.0f, 23.9f);
	glm::vec2 diff = glm::vec2(-16.0f, -13.0);
	int idx = 0;
	for (int i = 0; i < 2; i++)
	{
		p.x = 9.0f + (diff.x * i);
		for (int j = 0; j < 2; j++)
		{
			p.y = 17.0f + (diff.y * j);
			if (idx > MAX_POINT_LIGHT - 3)
				break;
			ptLight[idx++].position = p;
		}
	}


	//shadow map
	//dir 
	dirDepthMap.Generate(2048, 2048);
	dirLightObject.dirLightShadow.config.cam_far = 70.0f;
	//point
	ptDepthCube.Generate(1024, 1024);
}

void ParallaxExperimentalScene::DrawObjects(Shader& shader, bool apply_tex)
{
	shader.Bind();
	glm::mat model = glm::mat4(1.0f);


	//ground material
	if(apply_tex)
		MaterialShaderBindHelper(*floorMat, shader);
	//ground 
	shader.SetUniformMat4f("u_Model", groundWorldTrans);
	ground.Render();

	floorMat->baseMap->DisActivate();

	//draw plane 
	if (apply_tex)
		MaterialShaderBindHelper(*planeMat, shader);
	shader.SetUniformMat4f("u_Model", planeWorldTran);
	ground.Render();


	//draw blender shapes
	if (apply_tex)
		MaterialShaderBindHelper(*wallMat, shader);
	shader.SetUniformMat4f("u_Model", shapesTrans);
	blenderShapes->Draw();


	for (auto& e : sceneEntities)
		e->Draw(shader);


	if (apply_tex)
	{
		shader.SetUniform1i("u_UseNorMap", 0);
		floorMat->baseMap->DisActivate(); //this should unbind all textures

	}

	shader.UnBind();
}

void ParallaxExperimentalScene::LightPass()
{
	//------------------Update uniform GPU buffer Lights -----------------------------/ 
	long long int offset_pointer = 0;
	offset_pointer = 0;
	dirLightObject.dirlight.UpdateUniformBufferData(m_LightDataUBO, offset_pointer);
	for(int i = 0; i < MAX_POINT_LIGHT; i++)
		ptLight[i].UpdateUniformBufferData(m_LightDataUBO, offset_pointer);

}

void ParallaxExperimentalScene::ShadowPass()
{
	RenderCommand::CullFront();
	//--------------------Dir Light------------/
	shadowDepthShader.Bind();
	shadowDepthShader.SetUniform1i("u_IsOmnidir", 0);
	shadowDepthShader.SetUniformMat4f("u_LightSpaceMat", dirLightObject.dirLightShadow.GetLightSpaceMatrix());
	dirDepthMap.Write();
	RenderCommand::ClearDepthOnly();//clear the depth buffer 
	//SCENES/OBJECT TO RENDER
	DrawObjects(shadowDepthShader);
	dirDepthMap.UnBind();
	//--------------------Point Light---------------/
	shadowDepthShader.Bind();
	std::vector<glm::mat4> shadowMats = PointLightSpaceMatrix(ptLight[0].position, ptShadowConfig);
	//general shadowing values
	shadowDepthShader.SetUniform1i("u_IsOmnidir", 1);
	shadowDepthShader.SetUniform1f("u_FarPlane", ptShadowConfig.cam_far);
	shadowDepthShader.Bind();
	shadowDepthShader.SetUniformVec3("u_LightPos", ptLight[0].position);
	for (int f = 0; f < 6; ++f)
	{
		shadowDepthShader.SetUniformMat4f(("u_ShadowMatrices[" + std::to_string(f) + "]").c_str(), shadowMats[f]);
	}
	ptDepthCube.Write();//ready to write in the depth cube framebuffer for light "i"
	RenderCommand::ClearDepthOnly();//clear the depth buffer 
	DrawObjects(shadowDepthShader);
	ptDepthCube.UnBind();


	RenderCommand::CullBack();
	RenderCommand::Viewport(0, 0, window->GetWidth(), window->GetHeight());

}

void ParallaxExperimentalScene::MaterialShaderBindHelper(Material& mat, Shader& shader)
{
	shader.Bind();
	unsigned int tex_units = 0;
	shader.SetUniformVec3("u_Material.baseColour", mat.baseColour);
	if (mat.baseMap)
	{
		mat.baseMap->Activate(tex_units);
		shader.SetUniform1i("u_Material.baseMap", tex_units++);
	}
	bool has_nor = (mat.normalMap && useNor) ? true : false;
	if (has_nor)
	{
		mat.normalMap->Activate(tex_units);
		shader.SetUniform1i("u_Material.normalMap", tex_units++);
	}
	if (mat.parallaxMap)
	{
		mat.parallaxMap->Activate(tex_units);
		shader.SetUniform1i("u_Material.parallaxMap", tex_units++);
	}
	shader.SetUniform1i("u_UseNorMap", has_nor);
	shader.SetUniform1i("u_Material.shinness", mat.shinness);
	shader.SetUniform1i("u_Material.useParallax", mat.useParallax);
	shader.SetUniform1f("u_Material.parallax", mat.heightScale);
}

void ParallaxExperimentalScene::EntityDebugUI(Entity& entity)
{
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::PushID(entity.GetID());
	ImGui::Text("Name: %s, ID: %d", entity.GetName(), entity.GetID());
	ImGui::Text("Number of Children: %d", entity.GetChildren().size());
	ImGui::SeparatorText("Transform");
	glm::vec3 translate, euler, scale;
	MathsHelper::DecomposeTransform(entity.GetTransform(), translate, euler, scale);
	bool update = ImGui::DragFloat3("Translate", &translate[0], 0.2f);
	update |= ImGui::DragFloat3("Rotation", &euler[0], 0.2f);
	update |= ImGui::DragFloat3("Scale", &scale[0], 0.2f);
	if (update)
	{
		entity.SetLocalTransform(glm::translate(glm::mat4(1.0f), translate) *
							glm::toMat4(glm::quat(glm::radians(euler))) *
							glm::scale(glm::mat4(1.0f), scale));
	}
	ImGui::Text("Material Name: %s", entity.GetMaterial()->name);



	if (entity.GetChildren().size() > 0)
	{
		if (ImGui::TreeNode("Children"))
		{
			for (auto& e : entity.GetChildren())
				EntityDebugUI(*e);

			ImGui::TreePop();
		}
	}

	ImGui::PopID();
}
