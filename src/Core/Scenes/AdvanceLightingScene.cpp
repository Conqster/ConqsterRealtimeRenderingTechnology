#include "AdvanceLightingScene.h"

#include "SceneGraph/Model.h"
#include "SceneGraph/ModelLoader.h"
#include "Graphics/RendererErrorAssertion.h"

#include "External Libs/imgui/imgui.h"
#include "Util/FilePaths.h"

#include "Graphics/DebugGizmos.h"
#include "EventHandle.h"

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


	//for gamma correction 
	//glEnable(GL_FRAMEBUFFER_SRGB);

	//////////////////////
	// To do gamma correction manually
	// best way is to have a pipline which includes 
	// post-processing at the end then apply gamma correction 
	// to the post-process image 
	//////////////////////

	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 1.0f/*0.5f*/);

	CreateObjects();
}

void AdvanceLightingScene::OnUpdate(float delta_time)
{
	//object updating 


	for (auto& lb : lightObject)
	{
		//Using cross product 
		//vec_light2center << vec_l2c
		//glm::vec3 vec_l2c = glm::normalize(pointLights[0].position - pointLocalWorldPosition[0]);
		glm::vec3 vec_l2c = glm::normalize(lb.objectPosition - lb.light.position);
		///////////////////up///
		/////////////////#//////
		/////////////////#//////
		/////////////////#//////
		////////##########//////
		///vec_l2c////////#//////
		///////////////////#/////
		////////////////////#////
		/////////////////////#reflected dir///

		glm::vec3 movedir_wc_reflect = glm::vec3(0.0f);
		if(glm::length(vec_l2c) > 0.0f)
			movedir_wc_reflect = glm::cross(vec_l2c, glm::vec3(0.0f, 1.0f, 0.0f));
		lb.light.position += movedir_wc_reflect * lb.moveSpeed;
		//test_pos_light_1 = lightObject[i].light.position;
		//clamp pos for over shotting
		lb.light.position = glm::clamp(lb.light.position, lb.objectPosition + glm::vec3(-1.0f, 0.0f, -1.0f) * lb.childLightOffset, lb.objectPosition + glm::vec3(1.0f, 0.0f, 1.0f) * lb.childLightOffset);

	}

	/////////////////////////////////////////////
	// AABB Test update
	/////////////////////////////////////////////
	bool* keys = EventHandle::GetKeys();
	glm::vec3 translatePlayer = glm::vec3(0.0f);
	if (keys[GLFW_KEY_UP])
	{
		if (keys[GLFW_KEY_LEFT_SHIFT])
			translatePlayer += glm::vec3(0.0f, 1.0f, 0.0f) * playerTest.speed;
		else
			translatePlayer += glm::vec3(0.0f, 0.0f, 1.0f) * playerTest.speed;
	}
	if (keys[GLFW_KEY_DOWN])
	{
		if (keys[GLFW_KEY_DOWN] && keys[GLFW_KEY_LEFT_SHIFT])
			translatePlayer += glm::vec3(0.0f, -1.0f, 0.0f) * playerTest.speed;
		else
			translatePlayer += glm::vec3(0.0f, 0.0f, -1.0f) * playerTest.speed;
	}
	if (keys[GLFW_KEY_RIGHT])
		translatePlayer += glm::vec3(1.0f, 0.0f, 0.0f) * playerTest.speed;
	if (keys[GLFW_KEY_LEFT])
		translatePlayer += glm::vec3(-1.0f, 0.0f, 0.0f) * playerTest.speed;


	if (keys[GLFW_KEY_N])
		if(keys[GLFW_KEY_LEFT_SHIFT])
			playerTest.aabb.Scale(glm::vec3(-0.2f));
		else
			playerTest.aabb.Scale(glm::vec3(0.2f));

	if (keys[GLFW_KEY_V])
		if(keys[GLFW_KEY_LEFT_SHIFT])
			playerTest.aabb.Scale(glm::vec3(-0.2f, 0.0f, 0.0f));
		else
			playerTest.aabb.Scale(glm::vec3(0.2f, 0.0f, 0.0f));


	playerTest.aabb.Translate(translatePlayer);


	////////////////////
	// update light matrix  data
	////////////////////
	if (dirlight.castShadow)
	{
		dirLightShadow.UpdateProjMat();
		dirLightShadow.UpdateViewMatrix(dirlight.direction);
	}
	dirLightShadow.sampleWorldPos = playerTest.aabb.GetCenter() + glm::vec3(0.0f, 0.0f, 1.0) * playerTest.shadowOffset;


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





	//////////////////////////
	//  SECOND PASS for Testing shadow Pass
	//////////////////////////
	dirLightShadow.depthShader.Bind();
	glCullFace(GL_FRONT);
	ShadowPass();
	glCullFace(GL_BACK);
	glViewport(0, 0, window->GetWidth(), window->GetHeight()); //reset the view back just in case

	/////////////////////
	// First Pass : Draw Scene
	/////////////////////
	//general debug parameters for model shader
	modelShader.Bind();
	modelShader.SetUniform1i("u_DebugScene", debugScene);
	modelShader.SetUniform1i("u_DebugWcType", debugModelType);
	modelShader.SetUniform1i("u_DisableTex", disableTexture);
	modelShader.SetUniform1i("u_GammaCorrection", doGammaCorrection);
	modelShader.SetUniform1f("u_Gamma", gamma);
	///////////for shadow
	modelShader.SetUniformMat4f("u_LightSpaceMatrix", dirLightShadow.GetLightSpaceMatrix());
	/////////////////////////////////////
	//Quick hack
	/////////////////////////////////////
	if (dirlight.castShadow)
		dirLightShadow.depthMap.Read(1);
	else
		plainTex->Activate(1);

	modelShader.SetUniform1i("u_ShadowMap", 1);
	modelShader.SetUniform1i("u_ShadowSampleType", shadowSamplingType);
	LightPass(modelShader);
	DrawObjects(modelShader);
	
	//Pass Instance Objects
	LightPass(instancingShader);
	InstanceObjectPass();



	/////////////////////
	// Second Pass : Draw Debug normal
	/////////////////////
	debugShader.Bind();
	debugShader.SetUniform1f("u_NorDebugLength", normDebugLength);
	debugShader.SetUniformVec3("u_DebugColour", normDebugColour);
	debugShader.SetUniform1i("u_UseDebugColour", useDebugColour);
	debugShader.SetUniform1i("u_DebugPosColour", debugVertexPosColour);
	DrawObjects(debugShader);
	InstanceObjectPass(&debugShader);  //pass the debug shader for object instances


	//////////////////////////
	// Debug Pass
	//////////////////////////

	///////////////////////Shadow Depth View
	uint16_t win_width = window->GetWidth() * 0.25f; // 0.25f;
	uint16_t win_height = window->GetHeight() * 0.25f; // 0.25f;
	uint16_t x_offset = win_width * 0.6f,
		y_offset = win_height * 0.6f;
	uint16_t x_pos = window->GetWidth() - (win_width * 0.5f) - x_offset;
	uint16_t y_pos = window->GetHeight() - (win_height * 0.5f) - y_offset;

	glViewport(x_pos, y_pos, win_width, win_height);
	//No need to clear screen have it the the cover area

	screenShader.Bind();
	dirLightShadow.depthMap.Read(0);
	//quadAfterEffect.RenderDebugOutLine();
	glBindVertexArray(m_Quad.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	screenShader.SetUniform1f("u_Near", dirLightShadow.cam_near);
	screenShader.SetUniform1f("u_Far", dirLightShadow.cam_far);
	screenShader.UnBind();
	glViewport(0, 0, window->GetWidth(), window->GetHeight());
	//return;







	if (debugLightPos)
	{
		for (int i = 0; i < availablePtLightCount; i++)
		{
			auto& lb = lightObject[i];
			//DebugGizmos::DrawWireSphere(lb.objectPosition, 0.5f, lb.light.colour, 2.0f);
			//DebugGizmos::DrawSphere(lb.light.position, 0.1f, lb.light.colour);
			DebugGizmos::DrawCross(lb.objectPosition);
			DebugGizmos::DrawLine(lb.objectPosition, lb.light.position, lb.light.colour, 1.0f);
			DebugGizmos::DrawWireThreeDisc(lb.light.position, 2.0f, 10, lb.light.colour, 1.0f);
			DebugGizmos::DrawCross(lb.light.position, 1.0f, false, lb.light.colour);
		}
	}

	//Create new Player AABB
	DebugGizmos::DrawBox(playerTest.aabb, glm::vec3(1.0f, 0.0f, 0.0f), playerTest.debugThick);

	//test directional Shadow info 
	if (dirLightShadow.debugPara)
	{
		auto& ds = dirLightShadow;
		float dcv = ds.cam_offset + ds.cam_near * 0.5f; //dcv is the center/value between the near & far plane 
		glm::vec3 orthCamPos = ds.sampleWorldPos + (dirlight.direction * ds.cam_offset);
		glm::vec3 farPlane = orthCamPos + (glm::normalize(-dirlight.direction) * ds.cam_far);
		glm::vec3 nearPlane = orthCamPos + (glm::normalize(dirlight.direction) * ds.cam_near);
		DebugGizmos::DrawOrthoCameraFrustrm(orthCamPos, dirlight.direction,
			ds.cam_near, ds.cam_far, ds.cam_size,
			glm::vec3(0.0f, 1.0f, 0.0f));

		//Shadow Camera Sample Position 
		DebugGizmos::DrawCross(ds.sampleWorldPos);
		DebugGizmos::DrawLine(playerTest.aabb.GetCenter(), ds.sampleWorldPos, glm::vec3(1.0f, 0.0f, 0.0f), playerTest.debugThick);
	}


	if (debugSphereAABB)
	{
		AABB temp = sphere.GetAABB();
		glm::vec3 world_origin = glm::vec3(0.0f);
		glm::vec3 relative_pos = spheresPos[0] - world_origin;
		for(int i = 0; i < MAX_SPHERE; i++)
		{
			relative_pos = spheresPos[i] - temp.GetCenter();
			temp.Translate(relative_pos);
			temp.Scale(glm::vec3(1.0f) * (spheresScale[i] - 1.0f));
			DebugGizmos::DrawBox(temp, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		temp = sphere.GetAABB();
		for (auto& s : sphereInstancePos)
		{
			relative_pos = s - temp.GetCenter();
			temp.Translate(relative_pos);
			DebugGizmos::DrawBox(temp, glm::vec3(0.0f, 0.0f, 1.0f));
		}
	}

	if (debugBunnyAABB)
	{
		if (reCalBunnyAABB)
		{
			model_1->RecalculateAABBFromMesh();
			reCalBunnyAABB = false;
		}
		glm::vec3 world_origin = glm::vec3(0.0f);
		glm::vec3 relative_pos = bunnysPos[0] - world_origin;
		for (int i = 0; i < MAX_BUNNY_MODEL; i++)
		{
			AABB temp = model_1->GetAABB();
			relative_pos = bunnysPos[i] - world_origin;// temp.GetCenter();
			relative_pos += centerOffset;
			//relative_pos += glm::vec3(-0.026677, 0.095087, 0.008953);
			temp.Translate(relative_pos);
			//temp.Scale(glm::vec3(1.0f) * (bunnysScale[i] - substractScale));
			//temp.Scale(glm::vec3(0.2f) * (bunnysScale[i] - 1.0f));
			temp.Scale(glm::vec3(0.1593535) * (bunnysScale[i] - 1.0f));
			//temp.Scale(glm::vec3(-0.026677, 0.095087, 0.008953) * (bunnysScale[i] - 1.0f));
			//temp.Scale(glm::vec3(-0.204654, 0.831132, 0.093963) * (bunnysScale[i] - 1.0f));
			DebugGizmos::DrawBox(temp, glm::vec3(0.0f, 0.0f, 1.0f), 2.0f);
		}
	}

	if (debugModel2AABB)
	{
		AABB temp = model_2->GetAABB();
		glm::vec3 world_origin = glm::vec3(0.0f);
		glm::vec3 relative_pos = model_2Pos - world_origin;
		relative_pos += model2AABBCenterOffset;
		temp.Translate(relative_pos);
		temp.Scale(glm::vec3(model2AABBScale) * (model_2Scale - 1.0f));
		//temp.Translate(model2AABBCenterOffset);
		//temp.Scale(glm::vec3(1.0f) * model2AABBScale);
		DebugGizmos::DrawBox(temp, glm::vec3(0.0f, 0.0f, 1.0f), 1.5f);

		for (auto& m : model_2->GetMeshes())
		{
			AABB aabb = m.GetAABB();
			glm::vec3 relative_pos = model_2Pos - world_origin;
			relative_pos += model2AABBCenterOffset;
			aabb.Translate(model_2Pos);
			aabb.Scale(glm::vec3(model2AABBScale) * (model_2Scale - 1.0f));
			//aabb.Translate(model2AABBCenterOffset);
			//aabb.Scale(glm::vec3(1.0f) * model2AABBScale);
			DebugGizmos::DrawBox(aabb, glm::vec3(0.0f, 1.0f, 1.0f), 1.5f);
		}
	}


	if (debugCubeAABB)
	{
		AABB temp = cube.GetAABB();
		glm::vec3 world_origin = glm::vec3(0.0f);
		glm::vec3 relative_pos = cubesPos[0] - world_origin;
		temp.Translate(relative_pos);
		//temp.Scale(glm::vec3(0.5f) * (cubesScale[0] - 1.0f));
		//temp.Scale(glm::vec3(1.0f) * (cubesScale[0] - 1.0f));
		temp.Scale(glm::vec3(0.5f) * (cubesScale[0] - 1.0f));
		//temp.Scale(glm::vec3(0.577350f) * (cubesScale[0] - 1.0f));
		DebugGizmos::DrawBox(temp, glm::vec3(0.0f, 1.0f, 0.0f), 1.5f);
	}


	//Create & Debug AABB for all Spheres
	if (debugAllSphereAABB)
	{
		AABB aabb = AABB(spheresPos[0]);
		//get all pos/bounds for spheres 
		glm::vec3 world_origin = glm::vec3(0.0f);
		glm::vec3 relative_pos = spheresPos[0] - world_origin;
		AABB temp = sphere.GetAABB();
		//need to fix this the init is dependant on the origin
		relative_pos = spheresPos[0] - temp.GetCenter();
		temp.Translate(relative_pos);
		temp.Scale(glm::vec3(1.0f) * (spheresScale[0] - 1.0f));
		//aabb = temp;
		for (int i = 0; i < MAX_SPHERE; i++)
		{
			relative_pos = spheresPos[i] - temp.GetCenter();
			temp.Translate(relative_pos);
			temp.Scale(glm::vec3(1.0f) * (spheresScale[i] - 1.0f));
			aabb.Encapsulate(temp);
		}


		AABB aabb2 = AABB(sphereInstancePos[0]);
		temp = sphere.GetAABB();
		relative_pos = sphereInstancePos[0] - world_origin;
		temp = sphere.GetAABB();
		//need to fix this the init is dependant on the origin
		relative_pos = sphereInstancePos[0] - temp.GetCenter();
		temp.Translate(relative_pos);
		//aabb2 = temp;
		for (auto& s : sphereInstancePos)
		{
			relative_pos = s - temp.GetCenter();
			temp.Translate(relative_pos);
			aabb2.Encapsulate(temp);
		}

		//After all calculation draw AABB Debug
		DebugGizmos::DrawBox(aabb, glm::vec3(1.0f, 1.0f, 0.0f), 1.5f);
		DebugGizmos::DrawBox(aabb2, glm::vec3(0.0f, 1.0f, 1.0f), 1.5f);
		aabb.Encapsulate(aabb2);//combine tehe two AABB
		DebugGizmos::DrawBox(aabb, glm::vec3(0.4f, 0.2f, 1.0f), 1.5f);
	}
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

	ImGui::SeparatorText("Player AABB Test");
	ImGui::SliderFloat("AABB Test, move speed", &playerTest.speed, 0.0f, 3.0f, "%.1f");
	ImGui::SliderFloat("AABB Test, Debug Thickness", &playerTest.debugThick, 0.1f, 10.0f, "%.1f");
	ImGui::SliderFloat("AABB Test, shadow offset speed", &playerTest.shadowOffset, 0.0f, 20.0f, "%.1f");
	
	////////////////////////////////////////////////
	// SCENE OBJECTS
	////////////////////////////////////////////////
	ImGui::Spacing();
	if (ImGui::TreeNode("Scene Objects"))
	{
		ImGui::SeparatorText("Ground");
		ImGui::DragFloat3("Ground Pos", &groundPos[0], 0.1f);
		ImGui::SliderFloat("Ground Scale", &groundScale, 100.0f, 1000.0f);

		//ImGui::SeparatorText("Model 1");
		//ImGui::DragFloat3("Model 1 pos", &model_1Pos[0], 0.1f);
		//ImGui::SliderFloat("Model 1 Scale", &model_1Scale, 0.1f, 30.0f);
		
		if (ImGui::TreeNode("Bunny Model"))
		{
			ImGui::SeparatorText("Bunnies AABB Test");
			ImGui::Checkbox("Debug Bunnies AABB bounds", &debugBunnyAABB);
			ImGui::Checkbox("Recalculate Bunnies AABB bounds", &reCalBunnyAABB);
			ImGui::DragFloat3("Debug Center Offset", &centerOffset[0], 0.01f);
			//ImGui::SliderFloat("Debug AABB substract from scale", &substractScale, 0.0f, 20.0f);
			ImGui::SliderFloat("Debug AABB scale by", &scaleBunnyBy, -0.2f, 2.0f);

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
			ImGui::SeparatorText("Sphere AABB Test");
			ImGui::Checkbox("Debug Collective Sphere", &debugAllSphereAABB);
			ImGui::Checkbox("Debug Spheres AABB bounds", &debugSphereAABB);

			for (int i = 0; i < MAX_SPHERE; i++)
			{
				std::string label = "sphere index " + std::to_string(i);
				//ImGui::PushID(&cubesPos[i]);
				ImGui::SeparatorText(label.c_str());
				ImGui::DragFloat3((label + " pos").c_str(), &spheresPos[i][0], 0.1f);
				ImGui::SliderFloat((label + " scale").c_str(), &spheresScale[i], 0.1f, 10.f);
				//ImGui::PopID();
			}

			//INSTANCE SPHERE UI
			ImGui::Spacing();
			ImGui::Text("SPHERE INSTANCE");
			ImGui::Spacing();
			int usecount = (sphereInstancePos.size() < MAX_SPHERE_INSTANCE) ? sphereInstancePos.size() : MAX_SPHERE_INSTANCE;
			for (int i = 0; i < usecount; i++)
			{
				std::string label = "sphere instance index " + std::to_string(i);
				ImGui::SeparatorText(label.c_str());
				ImGui::DragFloat3((label + " pos").c_str(), &sphereInstancePos[i][0], 0.1f);
			}


			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Cubes"))
		{
			ImGui::SeparatorText("Cube AABB Test");
			ImGui::Checkbox("Debug Cubes AABB bounds", &debugCubeAABB);

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
		ImGui::Checkbox("Debug model 2 AABB bounds", &debugModel2AABB);
		ImGui::SliderFloat("Debug AABB by scale", &model2AABBScale, 0.0f, 20.0f);
		ImGui::DragFloat3("Model 2 AABB center offset", &model2AABBCenterOffset[0], 0.1f);


		ImGui::TreePop();
	}


	///////////////////////////////////////////
	// SCENE LIGHTS
	///////////////////////////////////////////
	ImGui::Spacing();
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::SeparatorText("Light Global Properties");
		ImGui::Checkbox("Do Gamma Correction", &doGammaCorrection);
		ImGui::SliderFloat("Gamma Value", &gamma, 1.9f, 2.5f);
		ImGui::SliderInt("Specular Shinness", &specShinness, 0, 128);
		ImGui::Checkbox("Debug Lights Pos", &debugLightPos);
		ImGui::Checkbox("Use Blinn-Phong", &useBlinnPhong);


		//////////////////////////////////////
		// Directional Light
		//////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Directional Light");
		ImGui::Checkbox("Enable Directional", &dirlight.enable);
		ImGui::SameLine();
		ImGui::Checkbox("Cast Shadow", &dirlight.castShadow);
		ImGui::DragFloat3("Light Direction", &dirlight.direction[0], 0.1f, -1.0f, 1.0f);
		ImGui::ColorEdit3("Dir Light colour" , &dirlight.colour[0]);
		ImGui::SliderFloat("Light ambinentIntensity", &dirlight.ambientIntensity, 0.0f, 1.0f);
		if (dirlight.castShadow)
		{
			if (ImGui::TreeNode("Shadow Camera Info"))
			{
				auto& shadow = dirLightShadow;
				ImGui::SliderFloat("Camera Near", &shadow.cam_near, 0.0f, shadow.cam_far - 0.5f);
				ImGui::SliderFloat("Camera Far", &shadow.cam_far, shadow.cam_near + 0.5f, 1000.0f);
				ImGui::SliderFloat("Camera Size", &shadow.cam_size, 0.0f, 200.0f);
				ImGui::DragFloat3("Sample Pos", &shadow.sampleWorldPos[0], 0.1f);
				ImGui::SliderFloat("Light Proj Offset", &shadow.cam_offset, 0.0f, 100.0f);
				ImGui::Checkbox("Debug Dir Shadow Para", &shadow.debugPara);

				static int cur_sel_type = 0;
				const char* element_name[] = { "PCF", "POISSON SAMPLING" };
				ImGui::Combo("Shadow Sampling Type", &cur_sel_type, element_name, IM_ARRAYSIZE(element_name));
				shadowSamplingType = (ShadowSamplingType)cur_sel_type;

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
			for (int i = 0; i < availablePtLightCount; i++)
			{
				//u_Lights[i].position....
				std::string label = "point idx: " + std::to_string(i);
				ImGui::SeparatorText(label.c_str());
				ImGui::Checkbox((label + " Enable light").c_str(), &lightObject[i].light.enable);

				ImGui::DragFloat3((label + " position").c_str(), &lightObject[i].objectPosition[0], 0.1f);
				ImGui::DragFloat3((label + " DEBUG position").c_str(), &lightObject[i].light.position[0], 0.1f);
				if (ImGui::SliderFloat((label + " light offset").c_str(), &lightObject[i].childLightOffset, 0.0f, 30.0f))
				{
					if(glm::length(lightObject[i].light.position) == glm::length(lightObject[i].objectPosition))
						lightObject[i].light.position = lightObject[i].objectPosition + (glm::vec3(1.0f, 0.0f, 0.0f) * lightObject[i].childLightOffset);
					else
					{
						glm::vec3 vec = glm::normalize(lightObject[i].light.position - lightObject[i].objectPosition);
						lightObject[i].light.position = vec * lightObject[i].childLightOffset;
					}
				}
				ImGui::SliderFloat((label + " move speed").c_str(), &lightObject[i].moveSpeed, 0.0f, 5.0f);

				ImGui::ColorEdit3((label + " colour").c_str(), &lightObject[i].light.colour[0]);
				ImGui::SliderFloat((label + " ambinentIntensity").c_str(), &lightObject[i].light.ambientIntensity, 0.0f, 1.0f);
				ImGui::SliderFloat((label + " constant attenuation").c_str(), &lightObject[i].light.attenuation[0], 0.0f, 1.0f);
				ImGui::SliderFloat((label + " linear attenuation").c_str(), &lightObject[i].light.attenuation[1], 0.0f, 1.0f);
				ImGui::SliderFloat((label + " quadratic attenuation").c_str(), &lightObject[i].light.attenuation[2], 0.0f, 1.0f);
			}

			ImGui::TreePop();
		}

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
		const char* element_name[] = {"Model Space", "Normal", "Model Normal", "Model Abs Normal","Model Colour", "Default Colour"};
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

	glDeleteVertexArrays(1, &m_Quad.VAO);



	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	DebugGizmos::Cleanup();
	Scene::OnDestroy();
}

void AdvanceLightingScene::CreateObjects()
{
	///////////////
	// Create Objects & model
	///////////////
	//model_2 = modelLoader.Load(FilePaths::Instance().GetPath("bunny"), true);
	model_2 = modelLoader.Load(FilePaths::Instance().GetPath("backpack"), true);
	model_1 = modelLoader.Load(FilePaths::Instance().GetPath("bunny"), true);
	//model_2 = modelLoader.Load(FilePaths::Instance().GetPath("electrical-charger"), true);
	//model_1 = modelLoader.Load("Assets/Textures/backpack/backpack.obj", true);

	GLint maxTextureSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	printf("Max Texture 2D size, %d", maxTextureSize);
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



	//////////////////////////////////////
	// CREATE SCREEN QUAD
	//////////////////////////////////////

	float quad_vertices[] = {

		// x   y		 u	   v
		-1.0f, 1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,	0.0f, 0.0f,
		1.0f, -1.0f,	1.0f, 0.0f,

		-1.0f, 1.0f,	0.0f, 1.0f,
		1.0f, -1.0,		1.0f, 0.0f,
		1.0f, 1.0,		1.0f, 1.0f
	};

	glGenVertexArrays(1, &m_Quad.VAO);
	glGenBuffers(1, &m_Quad.VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_Quad.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_Quad.VAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(quad_vertices[0]) * 4, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(quad_vertices[0]) * 4, (void*)(sizeof(quad_vertices[0]) * 2));

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

	//generate pos&scale for sphere
	//(0.0f, 0.0f, 5.0f)
	//move backwards (-Z) by 4 units
	origin = glm::vec3(0.0f, 0.0f, 5.0f);
	for (int i = 0; i < MAX_BUNNY_MODEL; i++)
	{
		bunnysPos[i] = origin + glm::vec3(0.0f, 0.0f, offset_units * i);
		bunnysScale[i] = 10.0f;
	}

	//generate pos&scale for sphere
	//(4.5f, 0.5f, 5.0f)
	//move backwards (-Z) by 4 units
	origin = glm::vec3(4.5f, 1.0f, 5.0f);
	for (int i = 0; i < MAX_SPHERE; i++)
	{
		spheresPos[i] = origin + glm::vec3(0.0f, 0.0f, offset_units * i);
		spheresScale[i] = 1.0f;
	}

	///////////////////////////
	// CREATE SPHERE INSTANCE OFFSET POS
	//////////////////////////
	origin = glm::vec3(-15.0f, 1.0f, 15.0f);
	glm::vec3 ins_pos = glm::vec3(0.0f);
	glm::vec3 offset = glm::vec3(4.0f, 4.0f, -4.0f);
	int count = 3;
	for (int y = 0; y < count; y++)
	{
		ins_pos.y = origin.y + ((float)y * offset.y);
		for (int x = 0; x < count; x++)
		{
			ins_pos.x = origin.x + ((float)x * offset.x);
			for (int z = 0; z < count; z++)
			{
				if (sphereInstancePos.size() >= MAX_SPHERE_INSTANCE)
					break;

				ins_pos.z = origin.z + ((float)z * offset.z);
				sphereInstancePos.push_back(ins_pos);
			}
		}
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
	//INSTANCING OBJECT SHADER
	ShaderFilePath instance_shader_file_path
	{
		"src/ShaderFiles/Learning/AdvanceLighting/InstancingVertex.glsl", //vertex shader
		"src/ShaderFiles/Learning/AdvanceLighting/ModelFrag.glsl", //fragment shader
	};
	instancingShader.Create("instance_shader", instance_shader_file_path);

	//////////////////////////////////Shadow Debuging Shader
	ShaderFilePath shadow_shader_file_path
	{
		"src/ShaderFiles/Learning/Debugger/DepthMapVertex.glsl", //vertex shader
		"src/ShaderFiles/Learning/Debugger/DepthMapFrag.glsl", //fragment shader
	};
	dirLightShadow.depthShader.Create("shadow_depth", shadow_shader_file_path);
	dirLightShadow.depthMap.Generate();


	//////////////////////////////////Shadow Debuging Shader
	ShaderFilePath point_shadow_shader_file_path
	{
		"src/ShaderFiles/Learning/Debugger/PointLightDepthVertex.glsl", //vertex shader
		"src/ShaderFiles/Learning/Debugger/PointLightDepthFrag.glsl", //fragment shader
		"src/ShaderFiles/Learning/Debugger/PointLightDepthGeometry.glsl", //geometry shader
	};
	lightObject[0].shadowData.depthShader.Create("point_shadow_depth", point_shadow_shader_file_path);
	lightObject[0].shadowData.depthMap.Generate();


	ShaderFilePath screen_shader_file_path
	{
		//"src/ShaderFiles/Learning/Debugger/DepthMapVertex.glsl", //vertex shader
		//"src/ShaderFiles/Learning/Debugger/DepthMapFrag.glsl", //fragment shader
		"src/ShaderFiles/Learning/ScreenFrameVertex.glsl", //vertex shader
		"src/ShaderFiles/Learning/ScreenFrameFrag.glsl", //vertex shader
	};
	screenShader.Create("shadow_depth", screen_shader_file_path);

	////////////////////////////////////////
	// CREATE CAMERA MAT UNIFORM BUFFER
	////////////////////////////////////////
	long long int buf_size = 2 * sizeof(glm::mat4) + sizeof(glm::vec2);   //to store view, projection & screenRes(vec2)
	m_CameraMatUBO.Generate(buf_size);


	//DebugGizmos::Startup();

	////////////////////////////////////////
	// CREATE TEXTURES 
	////////////////////////////////////////
	//brick texture 
	brickTex = new Texture(FilePaths::Instance().GetPath("brick")/*, TextureFormat::SRGBA*/);
	//plain texture
	plainTex = new Texture(FilePaths::Instance().GetPath("plain")/*, TextureFormat::SRGBA*/);
	//manchester-image
	manchesterTex = new Texture(FilePaths::Instance().GetPath("manchester-image")/*, TextureFormat::SRGBA*/);




	/////////////////////////////////////////
	// DEFINE LIGHT NECESSARY PROP
	/////////////////////////////////////////
	//directional light
	dirlight.ambientIntensity = 0.05f;
	dirlight.colour = glm::vec3(0.3f);
	dirlight.enable = true;
	//Point light
	origin = glm::vec3(0.0f, 3.0f, 0.0f);
	glm::vec3 colours[5] =
				{
					glm::vec3(0.3f, 0.3f, 1.0f),
					glm::vec3(1.0f, 0.3f, 0.3f),
					glm::vec3(0.3f, 0.0f, 0.3f),
					glm::vec3(0.3f, 1.0f, 0.3f),
					glm::vec3(0.737f, 0.925f, 0.0f)
				};
	offset_units = -4.0f;
	for (int i = 0; i < MAX_LIGHT; i++)
	{
		lightObject[i].objectPosition = origin + glm::vec3(0.0f, 0.0f, offset_units * i);
		if (i == 0)
		{
			lightObject[i].childLightOffset = 20.0f;
			lightObject[i].moveSpeed = 0.18f;
		}
		else if (i == 1)
		{
			lightObject[i].childLightOffset = 7.0f;
			lightObject[i].moveSpeed = 0.2f;
		}
		else if (i == 2)
		{
			lightObject[i].childLightOffset = 6.0f;
			lightObject[i].moveSpeed = 0.1f;
			lightObject[i].objectPosition = glm::vec3(-9.5f, 5.9f, 11.2f);
		}
		else if (i == 4)
		{
			lightObject[i].childLightOffset = 28.0f;
			lightObject[i].moveSpeed = 0.5f;
		}
		lightObject[i].light.position = (glm::vec3(1.0f, 0.0f, 0.0f) * lightObject[i].childLightOffset) + lightObject[i].objectPosition;
		lightObject[i].light.colour = (i < 5) ? colours[i] : glm::vec3(0.3f, 0.0f, 0.3f);
		lightObject[i].light.ambientIntensity = 0.05f;
		lightObject[i].light.enable = true;
		availablePtLightCount++;
	}
	//debugScene = true;
	debugModelType = MODEL_NORMAL;

	//Testing value
	gamma = 1.80f; //for game correction
	lightObject[3].light.enable = false;
	dirlight.direction = glm::vec3(-1.0f, 1.0f, -1.0f);

	playerTest.aabb.Translate(glm::vec3(-15.0f, 2.25f, -8.0f));

	//position cube in front of camera for testing 
	//cubesPos[0] = m_Camera->GetPosition() + m_Camera->GetFroward() * 15.0f;
}

void AdvanceLightingScene::DrawObjects(Shader& shader)
{
	shader.Bind();

	//ground 
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, groundPos);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f) * groundScale);
	shader.SetUniformMat4f("u_Model", model);
	//brickTex->Activate();
	//manchesterTex->Activate();
	plainTex->Activate();
	//ground 1
	ground.Render();
	//brickTex->DisActivate();
	plainTex->DisActivate();
	//manchesterTex->DisActivate();
	//modelShader.UnBind();


	//BUNNY MODELS
	plainTex->Activate();
	for (int i = 0; i < MAX_CUBE; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, bunnysPos[i]);
		model = glm::scale(model, glm::vec3(1.0f) * bunnysScale[i]);
		shader.SetUniformMat4f("u_Model", model);
		model_1->Draw();
	}
	plainTex->DisActivate();



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
		if(i == 0)
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
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
	shader.SetUniform1i("u_Blinn_Phong", useBlinnPhong);

	////////////////////////
	// Point Light 
	////////////////////////
	shader.SetUniform1i("u_Shininess", specShinness);
	shader.SetUniform1i("u_LightCount", availablePtLightCount + 1); //+1 for directional light
	//shader.SetUniform1i("u_LightCount", testLight);

	////
	// Directional Light
	////
	std::string name2 = "u_Lights[" + std::to_string(availablePtLightCount) + "].";
	shader.SetUniform1i((name2 + "is_directional").c_str(), 1);
	shader.SetUniform1i((name2 + "is_enable").c_str(), dirlight.enable);
	shader.SetUniformVec3((name2 + "direction").c_str(), dirlight.direction);
	shader.SetUniformVec3((name2 + "colour").c_str(), dirlight.colour);
	shader.SetUniform1f((name2 + "ambinentIntensity").c_str(), dirlight.ambientIntensity);



	for (int i = 0; i < availablePtLightCount; i++)
	{
		//u_Lights[i].position....
		std::string name = "u_Lights[" + std::to_string(i) + "].";
		shader.SetUniform1i((name + "is_directional").c_str(), 0);
		shader.SetUniform1i((name + "is_enable").c_str(), lightObject[i].light.enable);
		//shader.SetUniformVec3((name + "position").c_str(), pointLights[i].position);
		//convert to world space  pos
		shader.SetUniformVec3((name + "position").c_str(), lightObject[i].light.position);
		shader.SetUniformVec3((name + "colour").c_str(), lightObject[i].light.colour);
		shader.SetUniform1f((name + "ambinentIntensity").c_str(), lightObject[i].light.ambientIntensity);
		shader.SetUniformVec3f((name + "attenuation").c_str(), lightObject[i].light.attenuation);
	}



	shader.UnBind();

}

void AdvanceLightingScene::InstanceObjectPass(Shader* debug_shader)
{
	//if not debugging use normal calculation
	if (!debug_shader)
	{
		instancingShader.Bind();

		//generate parameters
		instancingShader.SetUniform1i("u_DebugScene", debugScene);
		instancingShader.SetUniform1i("u_DebugWcType", debugModelType);
		instancingShader.SetUniform1i("u_DisableTex", disableTexture);
		instancingShader.SetUniform1i("u_GammaCorrection", doGammaCorrection);
		instancingShader.SetUniform1f("u_Gamma", gamma);

		instancingShader.SetUniformMat4f("u_LightSpaceMatrix", dirLightShadow.GetLightSpaceMatrix());
		dirLightShadow.depthMap.Read(1);
		instancingShader.SetUniform1i("u_ShadowMap", 1);

		plainTex->Activate();
		glm::mat4 model = glm::mat4(1.0f); //reset model 
		instancingShader.SetUniformMat4f("u_Model", model);
		int usecount = (sphereInstancePos.size() < MAX_SPHERE_INSTANCE) ? sphereInstancePos.size() : MAX_SPHERE_INSTANCE;
		for (int i = 0; i < usecount; i++)
		{
			instancingShader.SetUniformVec3(("u_InstPosOffset[" + std::to_string(i) + "]").c_str(), sphereInstancePos[i]);
		}
		sphere.RenderInstances(usecount);
		instancingShader.UnBind();

		return;
	}

	//DEBUGGING FUNCTION

	debug_shader->Bind();

	glm::mat4 model = glm::mat4(1.0f);
	//glm::vec3 origin = sphereInstancePos[0]; //first locatio
	for (int i = 0; i < sphereInstancePos.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, sphereInstancePos[i]);
		debug_shader->SetUniformMat4f("u_Model", model);
		sphere.Render();
	}

	debug_shader->UnBind();

}

void AdvanceLightingScene::ShadowPass()
{
	dirLightShadow.depthShader.Bind();

	dirLightShadow.depthShader.SetUniformMat4f("u_LightSpaceMatrix", dirLightShadow.GetLightSpaceMatrix());

	
	dirLightShadow.depthMap.Write();
	glClear(GL_DEPTH_BUFFER_BIT);
	//SCENES/OBJECT TO RENDER
	DrawObjects(dirLightShadow.depthShader);
	InstanceObjectPass(&dirLightShadow.depthShader);
	dirLightShadow.depthMap.UnBind();

	dirLightShadow.depthShader.UnBind();


	//Move later
	//Experiment PointLigth shadow data update
	float aspect = 1.0f; //because width == height 
	float near = 1.0f; 
	float far = 25.0f; 
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), aspect, near, far);
	

	//Point Light 1 Shadow
	auto& lb_shadow = lightObject[0].shadowData;
	lb_shadow.depthShader.Bind();

	lb_shadow.proj = proj;
	std::vector<glm::mat4> shadowMats = lb_shadow.PointLightSpaceMatrix(lightObject[0].light.position);
	for (int f = 0; f < 6; ++f)
	{
		lb_shadow.depthShader.SetUniformMat4f(("u_ShadowMatrices[" + std::to_string(f) + "]").c_str(), shadowMats[f]);
	}
	lb_shadow.depthMap.Write();
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawObjects(lb_shadow.depthShader);
	InstanceObjectPass(&lb_shadow.depthShader);
	lb_shadow.depthMap.UnBind();
	lb_shadow.depthShader.UnBind();
}
