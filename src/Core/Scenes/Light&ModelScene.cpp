//#include "Light&ModelScene.h"
//
//#include "Graphics/Meshes/CubeMesh.h"
//#include "Graphics/Texture.h"
//
//#include "External Libs/imgui/imgui.h"
//
//
//void Light_ModelScene::OnInit(Window* window)
//{
//	Scene::OnInit(window);
//
//	window->UpdateProgramTitle("light Model Scene");
//
//	//TO:DO: take this opengl func out later into child classes 
//	//		 so each child could define their specific behaviour 
//	//glDisable(GL_DEPTH_TEST);
//	//glDisable(GL_CULL_FACE);
//
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//
//
//	if (!m_Camera)
//		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 1.0f/*0.5f*/);
//
//	ShaderFilePath shader_file{ "src/ShaderFiles/VertexLearningOpen.glsl","src/ShaderFiles/FragLearningOpen.glsl" };
//	m_MainShaderProgram.Create("light_model_shader", shader_file);
//
//	CreateObjects();
//	//SetupLights();
//}
//
//void Light_ModelScene::OnUpdate(float delta_time)
//{
//	OnRender();
//}
//
//void Light_ModelScene::OnRender()
//{
//	glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 0.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	if (useNewModel)
//	{
//		modelShader.Bind();
//		modelShader.SetUniformMat4f("u_View", m_Camera->CalViewMat());
//		modelShader.SetUniformVec3("u_ViewPos", m_Camera->GetPosition());
//		modelShader.SetUniform1f("u_GlobalAmbientStrength", test_GlobalAmbientStrength);
//		modelShader.SetUniform1f("u_GlobalDiffuseStrength", test_GlobalDiffuseStrength);
//		modelShader.SetUniform1f("u_GlobalSpecularStrength", test_GlobalSpecularStrength);
//		modelShader.SetUniformMat4f("u_Projection", m_Camera->CalculateProjMatrix(window->GetAspectRatio()));
//		glm::mat4 model1 = glm::mat4(1.0f);
//		model1 = glm::translate(model1, glm::vec3(0.0f));
//		//model1 = glm::rotate(model1, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//		//model1 = glm::scale(model1, glm::vec3(5.05f));
//		modelShader.SetUniform1i("u_Material.shininess", test_Material.shiniess);
//		modelShader.SetUniform1f("u_Material.emissionIntensity", newMaterialEmissionIntensity);
//		modelShader.SetUniformMat4f("u_Model", model1);
//		//Dir light
//		modelShader.SetUniform1i("u_DirectionalLight.lightBase.use", dir_Light.lightBase.use);
//		modelShader.SetUniformVec3("u_DirectionalLight.lightBase.ambient", dir_Light.lightBase.ambient);
//		modelShader.SetUniformVec3("u_DirectionalLight.lightBase.diffuse", dir_Light.lightBase.diffuse);
//		modelShader.SetUniformVec3("u_DirectionalLight.lightBase.specular", dir_Light.lightBase.specular);
//		modelShader.SetUniformVec3("u_DirectionalLight.direction", dir_Light.direction);
//		//modelShader.SetUniformVec3("u_DepthFogColour", depth_fogColour);
//		newModel.Draw(modelShader);
//	}
//
//
//	//return;
//
//	m_MainShaderProgram.Bind();
//	glBindVertexArray(VAO);	
//	m_MainShaderProgram.SetUniformMat4f("u_projection", m_Camera->CalculateProjMatrix(window->GetAspectRatio()));
//	m_MainShaderProgram.SetUniformMat4f("u_view", m_Camera->CalViewMat());
//	m_MainShaderProgram.SetUniformVec3("u_ViewPos", m_Camera->GetPosition());
//	m_MainShaderProgram.SetUniformVec3("u_DepthFogColour", depth_fogColour);
//
//	//m_MainShaderProgram.SetUniformVec3("u_ToyColour", test_ToyColour);
//
//
//	////Test triangle
//	glm::mat4 model = glm::mat4(1.0f);
//	//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//	//model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
//	//model = glm::scale(model, glm::vec3(5.0f));
//	//m_MainShaderProgram.SetUniformMat4f("u_model", model);
//	//glDrawArrays(GL_TRIANGLES, 0, 3);
//	//glDrawArrays(GL_LINE_LOOP, 0, 3);
//
//
//	//Point Light 
//
//	for (unsigned int i = 0; i < sizeof(pt_Lights) / sizeof(pt_Lights[0]); i++)
//	{
//		char light_ambient[64];
//		sprintf_s(light_ambient, "u_PointLights[%zu].lightBase.ambient", i);
//		m_MainShaderProgram.SetUniformVec3(light_ambient, pt_Lights[i].lightBase.ambient);
//		char light_diffuse[64];
//		sprintf_s(light_diffuse, "u_PointLights[%zu].lightBase.diffuse", i);
//		m_MainShaderProgram.SetUniformVec3(light_diffuse, pt_Lights[i].lightBase.diffuse);
//		char light_specular[64];
//		sprintf_s(light_specular, "u_PointLights[%zu].lightBase.specular", i);
//		m_MainShaderProgram.SetUniformVec3(light_specular, pt_Lights[i].lightBase.specular);
//		char use_Light[64];
//		sprintf_s(use_Light, "u_PointLights[%zu].lightBase.use", i);
//		m_MainShaderProgram.SetUniform1i(use_Light, pt_Lights[i].lightBase.use);
//		char light_position[64];
//		sprintf_s(light_position, "u_PointLights[%zu].position", i);
//		m_MainShaderProgram.SetUniformVec3(light_position, pt_Lights[i].position);
//		char light_constant[64];
//		sprintf_s(light_constant, "u_PointLights[%zu].constant", i);
//		m_MainShaderProgram.SetUniform1f(light_constant, pt_Lights[i].constant);
//		char light_linear[64];
//		sprintf_s(light_linear, "u_PointLights[%zu].linear", i);
//		m_MainShaderProgram.SetUniform1f(light_linear, pt_Lights[i].linear);
//		char light_quadratic[64];
//		sprintf_s(light_quadratic, "u_PointLights[%zu].quadratic", i);
//		m_MainShaderProgram.SetUniform1f(light_quadratic, pt_Lights[i].quadratic);
//
//	}
//
//
//	//Spot Light
//	m_MainShaderProgram.SetUniformVec3("u_SpotLight.pointLightBase.lightBase.ambient", spot_light.pointLightBase.lightBase.ambient);
//	m_MainShaderProgram.SetUniformVec3("u_SpotLight.pointLightBase.lightBase.diffuse", spot_light.pointLightBase.lightBase.diffuse);
//	m_MainShaderProgram.SetUniformVec3("u_SpotLight.pointLightBase.lightBase.specular", spot_light.pointLightBase.lightBase.specular);
//	m_MainShaderProgram.SetUniform1i("u_SpotLight.pointLightBase.lightBase.use", spot_light.pointLightBase.lightBase.use);
//
//	m_MainShaderProgram.SetUniformVec3("u_SpotLight.pointLightBase.position", spot_light.pointLightBase.position);
//	m_MainShaderProgram.SetUniform1f("u_SpotLight.pointLightBase.constant", spot_light.pointLightBase.constant);
//	m_MainShaderProgram.SetUniform1f("u_SpotLight.pointLightBase.linear", spot_light.pointLightBase.linear);
//	m_MainShaderProgram.SetUniform1f("u_SpotLight.pointLightBase.quadratic", spot_light.pointLightBase.quadratic);
//
//	m_MainShaderProgram.SetUniformVec3("u_SpotLight.direction", spot_light.direction);
//	m_MainShaderProgram.SetUniform1f("u_SpotLight.innerCutoffAngle", glm::cos(glm::radians(spot_light.innerCutoffAngle)));
//	m_MainShaderProgram.SetUniform1f("u_SpotLight.outerCutoffAngle", glm::cos(glm::radians(spot_light.outerCutoffAngle)));
//
//	m_MainShaderProgram.SetUniform1i("u_LightMap", 5);
//	cube_SpecularMap_Test->Activate(5);
//
//	//Test cube 
//	m_MainShaderProgram.SetUniform1i("u_DirectionalLight.lightBase.use", dir_Light.lightBase.use);
//	m_MainShaderProgram.SetUniform1f("u_GlobalAmbientStrength", test_GlobalAmbientStrength);
//	m_MainShaderProgram.SetUniform1f("u_GlobalDiffuseStrength", test_GlobalDiffuseStrength);
//	m_MainShaderProgram.SetUniform1f("u_GlobalSpecularStrength", test_GlobalSpecularStrength);
//	//m_MainShaderProgram.SetUniform1i("u_SpecularPower", test_SpecularPower);
//	//Cube material 
//	//int diffuseMap;
//	//int specularMap;
//	//int emissionMap;
//	///*float*/ int shiniess;
//
//	////stored as int foe simplity
//	//bool invertSpecularMap;
//	//glm::vec3 emissionColour;
//	//float emissionStrength;
//	//bool emit;
//	m_MainShaderProgram.SetUniform1i("u_Material.diffuseMap", test_Material.diffuseMap);
//	m_MainShaderProgram.SetUniform1i("u_Material.specularMap", test_Material.specularMap);
//	m_MainShaderProgram.SetUniform1i("u_Material.emissionMap", test_Material.emissionMap);
//	m_MainShaderProgram.SetUniform1i("u_Material.shininess", test_Material.shiniess);
//	m_MainShaderProgram.SetUniform1i("u_Material.invertSpecularMap", test_Material.invertSpecularMap);
//	m_MainShaderProgram.SetUniformVec3("u_Material.emissionColour", test_Material.emissionColour);
//	m_MainShaderProgram.SetUniform1f("u_Material.emissionMapStrength", test_Material.emissionStrength);
//	m_MainShaderProgram.SetUniform1i("u_Material.emit", test_Material.emit);
//	m_MainShaderProgram.SetUniform1i("u_IsLight", 0);
//	// render the cube
//	cube_DiffuseMap_Test->Activate(test_Material.diffuseMap);
//	cube_SpecularMap_Test->Activate(test_Material.specularMap);
//	cube_EmissionMap_Test->Activate(test_Material.emissionMap);
//	//cube_Test->Render();
//
//	for (unsigned int i = 0; i < 10; i++)
//	{
//		model = glm::mat4(1.0f);
//		model = glm::translate(model, cubePositions[i]);
//		float angle = 20.0f * i;
//		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//		model = glm::scale(model, glm::vec3(1.5f));
//		m_MainShaderProgram.SetUniformMat4f("u_model", model);
//		glBindVertexArray(cubeVAO);
//		glDrawArrays(GL_TRIANGLES, 0, 36);
//	}
//
//	cube_DiffuseMap_Test->DisActivate();
//	cube_SpecularMap_Test->DisActivate();
//	cube_EmissionMap_Test->DisActivate();
//
//
//
//	//m_MainShaderProgram.SetUniformVec3("u_light.ambient", test_Light.ambient);
//	m_MainShaderProgram.SetUniformVec3("u_DirectionalLight.lightBase.ambient", dir_Light.lightBase.ambient);
//	m_MainShaderProgram.SetUniformVec3("u_DirectionalLight.lightBase.diffuse", dir_Light.lightBase.diffuse);
//	m_MainShaderProgram.SetUniformVec3("u_DirectionalLight.lightBase.specular", dir_Light.lightBase.specular);
//	m_MainShaderProgram.SetUniformVec3("u_DirectionalLight.direction", dir_Light.direction);
//	m_MainShaderProgram.SetUniform1i("u_IsLight", 1);
//
//	//Test cube as point light 
//	for (auto& pt_light : pt_Lights)
//	{
//		model = glm::mat4(1.0f);
//		model = glm::translate(model, pt_light.position);  // This is just a debug location for nowe
//		model = glm::scale(model, glm::vec3(0.15f));
//		m_MainShaderProgram.SetUniformVec3("u_LightColour", pt_light.lightBase.ambient/*test_LightColour*/);
//		m_MainShaderProgram.SetUniformMat4f("u_model", model);
//
//		cube_Test->Render();
//	}
//
//
//	//Test cube as spot light 
//	if (false)
//	{
//		model = glm::mat4(1.0f);
//		model = glm::translate(model, spot_light.pointLightBase.position);  // This is just a debug location for nowe
//		model = glm::scale(model, glm::vec3(0.15f));
//		m_MainShaderProgram.SetUniformVec3("u_LightColour", spot_light.pointLightBase.lightBase.ambient);
//		m_MainShaderProgram.SetUniformMat4f("u_model", model);
//		cube_Test->Render();
//	}
//
//
//	glBindVertexArray(0);
//	//std::cout << "Trying to render stuffs!!!!!!!!!! \n";
//}
//
//void Light_ModelScene::OnRenderUI()
//{
//
//	ImGui::Begin("Debug");
//
//	//CAMERA
//	ImGui::SeparatorText("Camera info");
//	ImGui::Text("Position x: %f, y: %f, z: %f",
//		m_Camera->GetPosition().x,
//		m_Camera->GetPosition().y,
//		m_Camera->GetPosition().z);
//
//	ImGui::Text("Pitch: %f", m_Camera->Ptr_Pitch());
//	ImGui::Text("Yaw: %f", m_Camera->Ptr_Yaw());
//
//
//	if (ImGui::TreeNode("Camera Properties"))
//	{
//		ImGui::SliderFloat("Move Speed", m_Camera->Ptr_MoveSpeed(), 5.0f, 50.0f);
//		ImGui::SliderFloat("Rot Speed", m_Camera->Ptr_RotSpeed(), 0.0f, 2.0f);
//
//		float window_width = window->GetWidth();
//		float window_height = window->GetHeight();
//		static glm::mat4 test_proj;
//
//		bool update_camera_proj = false;
//
//		update_camera_proj = ImGui::SliderFloat("FOV", m_Camera->Ptr_FOV(), 0.0f, 179.0f, "%.1f");
//		update_camera_proj += ImGui::DragFloat("Near", m_Camera->Ptr_Near(), 0.1f, 0.1f, 50.0f, "%.1f");
//		update_camera_proj += ImGui::DragFloat("Far", m_Camera->Ptr_Far(), 0.1f, 0.0f, 500.0f, "%.1f");
//
//		if (update_camera_proj)
//		{
//			glm::mat4 new_proj = m_Camera->CalculateProjMatrix(window->GetAspectRatio());
//			//m_MainRenderer2.UpdateShaderViewProjection(new_proj);
//		}
//
//
//
//		ImGui::TreePop();
//	}
//
//
//	///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	/////////////////////////////////////////SCENE PROPERTIES//////////////////////////////////////////////////
//	///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ImGui::SeparatorText("Scene Properties");
//
//	if (ImGui::Checkbox("Lock Cursor", window->Ptr_LockCursorFlag()))
//	{
//		//TO-DO: Below is not ideal but might chnage later / dispose
//		*window->Ptr_LockCursorFlag() = !(*window->Ptr_LockCursorFlag());
//		window->ToggleLockCursor();
//	}
//	ImGui::ColorEdit3("clear Screen Colour", &m_ClearScreenColour[0]);
//	ImGui::Spacing();
//	ImGui::ColorEdit3("Fog Depth Colour", &depth_fogColour[0]);
//
//	ImGui::SeparatorText("Playing Around");
//	
//		ImGui::ColorEdit3("LightColour", &test_LightColour[0]);
//		ImGui::SliderFloat("Material Emissive Intensity", &newMaterialEmissionIntensity, 0.0f, 10.0f, "%.1f");
//		ImGui::SeparatorText("Directional Light Prop");
//		ImGui::Checkbox("Use Directional Light", &dir_Light.lightBase.use);
//		ImGui::DragFloat3("Just for Fun", &test_LightColour[0], 0.1f);
//		ImGui::ColorEdit3("Light Ambient", &dir_Light.lightBase.ambient[0]);
//		ImGui::ColorEdit3("Light Diffuse", &dir_Light.lightBase.diffuse[0]);
//		ImGui::ColorEdit3("Light Specular", &dir_Light.lightBase.specular[0]);
//		ImGui::SliderFloat3("Light Dir", &dir_Light.direction[0], -1.0f, 1.0f);
//	
//	
//		if (ImGui::TreeNode("Cube Position"))
//		{
//			for (unsigned int i = 0; i < (sizeof(cubePositions) / sizeof(cubePositions[0])); i++)
//			{
//				std::string label = "cube index " + std::to_string(i);
//				ImGui::PushID(&cubePositions[i]);
//				ImGui::DragFloat3(label.c_str(), &cubePositions[i][0], 0.1f);
//				ImGui::PopID();
//			}
//	
//			ImGui::TreePop();
//		}
//	
//		if (ImGui::TreeNode("Point Light"))
//		{
//			for (unsigned int i = 0; i < sizeof(pt_Lights) / sizeof(pt_Lights[0]); i++)
//			{
//				ImGui::PushID(&pt_Lights[i]);
//				std::string label = "point light " + std::to_string(i);
//				ImGui::SeparatorText(label.c_str());
//				ImGui::Checkbox("Use Point Light", &pt_Lights[i].lightBase.use);
//				ImGui::Spacing();
//				ImGui::ColorEdit3("Point Light Ambient", &pt_Lights[i].lightBase.ambient[0]);
//				ImGui::ColorEdit3("Point Light Diffuse", &pt_Lights[i].lightBase.diffuse[0]);
//				ImGui::ColorEdit3("Point Light Specular", &pt_Lights[i].lightBase.specular[0]);
//				ImGui::DragFloat3("Point Light Position", &pt_Lights[i].position[0], 0.1f);
//				ImGui::Spacing();
//				ImGui::SliderFloat("Point Constant", &pt_Lights[i].constant, 0.0f, 1.0f);
//				ImGui::SliderFloat("Point Linear", &pt_Lights[i].linear, 0.0f, 1.0f);
//				ImGui::SliderFloat("Point Quadratic", &pt_Lights[i].quadratic, 0.0f, 1.0f);
//				ImGui::PopID();
//			}
//	
//			ImGui::TreePop();
//		}
//	
//		//ImGui::SeparatorText("Point Light Prop");
//		//ImGui::Checkbox("Use Point Light", &m_LearningRendering.pt_Light.lightBase.use);
//		//ImGui::Spacing();
//		//ImGui::ColorEdit3("Point Light Ambient", &m_LearningRendering.pt_Light.lightBase.ambient[0]);
//		//ImGui::ColorEdit3("Point Light Diffuse", &m_LearningRendering.pt_Light.lightBase.diffuse[0]);
//		//ImGui::ColorEdit3("Point Light Specular", &m_LearningRendering.pt_Light.lightBase.specular[0]);
//		//ImGui::DragFloat3("Point Light Position", &m_LearningRendering.pt_Light.position[0], 0.1f);
//		//ImGui::Spacing();
//		//ImGui::SliderFloat("Point Constant", &m_LearningRendering.pt_Light.constant, 0.0f, 1.0f);
//		//ImGui::SliderFloat("Point Linear", &m_LearningRendering.pt_Light.linear, 0.0f, 1.0f);
//		//ImGui::SliderFloat("Point Quadratic", &m_LearningRendering.pt_Light.quadratic, 0.0f, 1.0f);
//	
//		ImGui::SeparatorText("Spot Light Prop");
//		ImGui::Checkbox("Use Spot Light", &spot_light.pointLightBase.lightBase.use);
//		ImGui::Spacing();
//		ImGui::ColorEdit3("Spot Light Ambient", &spot_light.pointLightBase.lightBase.ambient[0]);
//		ImGui::ColorEdit3("Spot Light Diffuse", &spot_light.pointLightBase.lightBase.diffuse[0]);
//		ImGui::ColorEdit3("Spot Light Specular", &spot_light.pointLightBase.lightBase.specular[0]);
//		ImGui::DragFloat3("Spot Light Position", &spot_light.pointLightBase.position[0], 0.1f);
//		ImGui::Spacing();
//		ImGui::SliderFloat("Spot Constant", &spot_light.pointLightBase.constant, 0.0f, 1.0f);
//		ImGui::SliderFloat("Spot Linear", &spot_light.pointLightBase.linear, 0.0f, 1.0f);
//		ImGui::SliderFloat("Spot Quadratic", &spot_light.pointLightBase.quadratic, 0.0f, 1.0f);
//		ImGui::Spacing();
//		ImGui::SliderFloat3("Spot Light direction", &spot_light.direction[0], -1.0f, 1.0f);
//		spot_light.pointLightBase.position = m_Camera->GetPosition() + m_Camera->GetFroward();
//		spot_light.direction = m_Camera->GetFroward();
//		ImGui::SliderFloat("Inner Cutoff angle", &spot_light.innerCutoffAngle, 0.0f,spot_light.outerCutoffAngle, "%.1f");
//		ImGui::SliderFloat("Outer Cutoff angle", &spot_light.outerCutoffAngle,spot_light.innerCutoffAngle, 90.0f, "%.1f");
//		//ImGui::SliderAngle("Cutoff angle", &m_LearningRendering.spot_light.cutoffAngle, 0.0f);
//	
//		ImGui::SeparatorText("Toy Properties");
//		//ImGui::ColorEdit3("Ambient colour", &m_LearningRendering.test_Material.ambient[0]);
//		//ImGui::ColorEdit3("Diffuse Colour", &m_LearningRendering.test_Material.diffuse[0]);
//		//ImGui::ColorEdit3("Specular Colour", &m_LearningRendering.test_Material.specular[0]);
//		//Might play with this
//		ImGui::InputInt("Diffuse Map Slot: ", &test_Material.diffuseMap);
//		if (test_Material.diffuseMap < 0)
//			test_Material.diffuseMap = 0;
//		ImGui::InputInt("Specular Map Slot: ", &test_Material.specularMap);
//		if (test_Material.specularMap < 0)
//			test_Material.specularMap = 0;
//		ImGui::InputInt("Emission Map Slot: ", &test_Material.emissionMap);
//		if (test_Material.emissionMap < 0)
//			test_Material.emissionMap = 0;
//		ImGui::SliderInt("Specular Shininess", &test_Material.shiniess, 0, 256 * 2);
//		ImGui::Checkbox("Invert Specular Map", &test_Material.invertSpecularMap);
//		ImGui::ColorEdit3("Emission Colour", &test_Material.emissionColour[0]);
//		ImGui::SliderFloat("Emission Strength", &test_Material.emissionStrength, 0.0f, 10.0f);
//		ImGui::Checkbox("Emit", &test_Material.emit);
//	
//		ImGui::SeparatorText("Global Prop");
//		ImGui::SliderFloat("Ambient", &test_GlobalAmbientStrength, 0.0f, 10.0f);
//		ImGui::SliderFloat("Diffuse", &test_GlobalDiffuseStrength, 0.0f, 10.0f);
//		ImGui::SliderFloat("Specular Strength", &test_GlobalSpecularStrength, 0.0f, 10.0f);
//	
//	
//		//if (ImGui::TreeNode("Cube Position"))
//		//{
//		//	for (unsigned int i = 0; i < (sizeof(cubePositions) / sizeof(cubePositions[0])); i++)
//		//	{
//		//		std::string label = "cube index " + std::to_string(i);
//		//		ImGui::PushID(&cubePositions[i]);
//		//		ImGui::DragFloat3(label.c_str(), &cubePositions[i][0], 0.1f);
//		//		ImGui::PopID();
//		//	}
//	
//		//	ImGui::TreePop();
//		//}
//
//	ImGui::End();
//}
//
//void Light_ModelScene::OnDestroy()
//{
//
//
//	Scene::OnDestroy();
//}
//
//Light_ModelScene::~Light_ModelScene()
//{
//	printf("Destroying learning Renderer!!!!!!!\n");
//
//	delete cube_Test;
//	cube_Test = nullptr;
//
//	delete cube_DiffuseMap_Test;
//	cube_DiffuseMap_Test = nullptr;
//
//	delete cube_SpecularMap_Test;
//	cube_SpecularMap_Test = nullptr;
//
//	delete cube_EmissionMap_Test;
//	cube_EmissionMap_Test = nullptr;
//}
//
//void Light_ModelScene::CreateObjects()
//{
//	if (useNewModel)
//	{
//		newModel.GenModel("Assets/Textures/backpack/backpack.obj");
//		//newModel.GenModel("Assets/Textures/sci-fi_electrical_charger/scene.gltf", true);  //change scale to 0.1f
//		//newModel.GenModel("Assets/Textures/chips_shop_fbx/source/El Chips.fbx");  //change scale to 0.1f
//		//newModel.GenModel("Assets/Textures/subway_metro_train/scene.gltf");  //change scale to 0.1f
//		//newModel.GenModel("Assets/Textures/UDIM_monster/UDIM_monster/udim-monster.obj");  //change scale to 0.1f
//		//newModel.GenModel("Assets/Textures/bugatti/bugatti.obj");
//		//newModel.GenModel("C:/Users/okeja/Desktop/Personal Project/3D-Rendering/Assets/Textures/backpack/backpack.obj");
//		ShaderFilePath shader_file{ "src/ShaderFiles/ModelVertexShader.glsl" ,"src/ShaderFiles/ModelFragShader.glsl" };
//		AdditionalShader(shader_file, glm::mat4(1.0f));
//	}
//
//
//	cube_Test = new CubeMesh();
//	cube_Test->Create();
//
//	//static Texture new_texture("Assets/Textures/container.pn
//	// g");
//	//cube_AmbientMap_Test = &new_texture;
//
//	cube_DiffuseMap_Test = new Texture("Assets/Textures/container.png");
//	cube_SpecularMap_Test = new Texture("Assets/Textures/container_specular.png");
//	cube_EmissionMap_Test = new Texture("Assets/Textures/container_emit_matrix.jpg");
//	//texture_Test->LoadTexture("Assets/Textures/brick.png");
//
//	float vertices[] =
//	{
//		 0.0f,  0.5f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,
//		-1.0f, -1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,
//		 1.0f, -1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,
//	};
//
//
//
//
//	glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//
//
//	glGenBuffers(1, &VBO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 4));
//
//
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//	////////////////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////////////////
//
//	float cube_vertices[] = {
//		// positions          // normals           // texture coords
//		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
//		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
//		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
//		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
//		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
//		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
//
//		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
//		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
//		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
//		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
//		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
//		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
//
//		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
//		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
//		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
//		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
//		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
//		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
//
//		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
//		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
//		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
//		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
//		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
//		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
//
//		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
//		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
//		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
//		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
//		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
//		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
//
//		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
//		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
//		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
//		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
//		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
//		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
//	};
//	// first, configure the cube's VAO (and VBO)
//	glGenVertexArrays(1, &cubeVAO);
//	glGenBuffers(1, &cubeVBO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
//
//	glBindVertexArray(cubeVAO);
//
//	// position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	// normal attribute
//	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(3);
//
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//	glEnableVertexAttribArray(2);
//
//}
//
//void Light_ModelScene::AdditionalShader(const ShaderFilePath& shader_file, const glm::mat4& viewProj)
//{
//	modelShader.CreateFromFile(shader_file);
//	modelShader.Bind();
//	modelShader.SetUniformMat4f("u_projection", viewProj);
//}
