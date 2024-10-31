#include "ParallaxExperimentalScene.h"

#include "External Libs/imgui/imgui.h"
#include "Util/FilePaths.h"
#include "Util/MathsHelpers.h"

#include <glm/gtx/quaternion.hpp>

void ParallaxExperimentalScene::SetWindow(Window* window)
{
	this->window = window;
}

void ParallaxExperimentalScene::OnInit(Window* window)
{
	Scene::OnInit(window);

	window->UpdateProgramTitle("Parallax Experimental Scene");

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);



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
	if (dirLightObject.dirlight.castShadow)
	{
		dirLightObject.dirLightShadow.UpdateProjMat();
		dirLightObject.dirLightShadow.UpdateViewMatrix(dirLightObject.sampleWorldPos, 
													   dirLightObject.dirlight.direction,
													   dirLightObject.cam_offset);
	}


	//point shadow far update 
	float shfar = ptShadowConfig.cam_far;
	ptLight.shadow_far = shfar;
	//for (auto& pt : lightObject)


	OnRender();
}

void ParallaxExperimentalScene::OnRender()
{
	GLCall(glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	////////////////////////////////////////
	// UPDATE UNIFORM BUFFERs
	////////////////////////////////////////
	//------------------Camera Matrix Data UBO-----------------------------/
	m_CamMatUBO.SetSubDataByID(&(m_Camera->CalculateProjMatrix(window->GetAspectRatio())[0][0]), sizeof(glm::mat4), 0);
	m_CamMatUBO.SetSubDataByID(&(m_Camera->CalViewMat()[0][0]), sizeof(glm::mat4), sizeof(glm::mat4));


	LightPass(); //Update Light data buffer
	ShadowPass();


	/////////////////////
	// First Pass : Draw Scene
	/////////////////////
	//for shadow
	modelShader.Bind();

	//would be good to have this in camera buffer
	modelShader.SetUniformVec3("u_ViewPos", m_Camera->GetPosition());

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


	DrawObjects(modelShader, true);



	//Debuging 
	DebugGizmos::DrawWireThreeDisc(ptLight.position, 2.0f, 10, ptLight.colour, 1.0f);
	DebugGizmos::DrawCross(ptLight.position);

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
		ImGui::SeparatorText("Ground");
		ImGui::DragFloat3("Ground Pos", &groundPos[0], 0.1f);
		ImGui::SliderFloat("Ground Scale", &groundScale, 100.0f, 1000.0f);
		ImGui::Checkbox("Use Normal Map", &useNor);

		ImGui::SeparatorText("Plane");


		ImGui::TreePop();
	}

	///////////////////////////////////////////
	// SCENE LIGHTS
	///////////////////////////////////////////
	ImGui::Spacing();
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::SeparatorText("Light Global Properties");
		//ImGui::Checkbox("Debug Lights Pos", &debugLightPos);


		//////////////////////////////////////
		// Directional Light
		//////////////////////////////////////
		ImGui::Spacing();
		ImGui::SeparatorText("Directional Light");
		ImGui::Checkbox("Enable Directional", &dirLightObject.dirlight.enable);
		ImGui::SameLine();
		ImGui::Checkbox("Cast Shadow", &dirLightObject.dirlight.castShadow);
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
			if (ImGui::TreeNode("Shadow Camera Info"))
			{
				auto& shadow = ptShadowConfig;
				ImGui::Checkbox("Debug Pt Lights", &shadow.debugLight);
				ImGui::SliderFloat("Pt Shadow Camera Near", &shadow.cam_near, 0.0f, shadow.cam_far - 0.5f);
				ImGui::SliderFloat("Pt Shadow Camera Far", &shadow.cam_far, shadow.cam_near + 0.5f, 80.0f);
				ImGui::TreePop();
			}


				//u_Lights[i].position....
			std::string label = "point light: ";
			ImGui::SeparatorText(label.c_str());
			ImGui::Checkbox((label + " Enable light").c_str(), &ptLight.enable);
			ImGui::SameLine();
			ImGui::Checkbox((label + " Enable light").c_str(), &ptLight.castShadow);

			ImGui::DragFloat3((label + " position").c_str(), &ptLight.position[0], 0.1f);

			ImGui::ColorEdit3((label + " colour").c_str(), &ptLight.colour[0]);
			ImGui::SliderFloat((label + " ambinentIntensity").c_str(), &ptLight.ambientIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat((label + " diffuseIntensity").c_str(), &ptLight.diffuseIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat((label + " specIntensity").c_str(), &ptLight.specularIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat((label + " constant attenuation").c_str(), &ptLight.attenuation[0], 0.0f, 1.0f);
			ImGui::SliderFloat((label + " linear attenuation").c_str(), &ptLight.attenuation[1], 0.0f, 1.0f);
			ImGui::SliderFloat((label + " quadratic attenuation").c_str(), &ptLight.attenuation[2], 0.0f, 1.0f);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	ImGui::End();


	ImGui::Begin("Material Test");
	ImGui::Text("1. %s", floorMat.name);
	ImGui::ColorEdit3("Colour", &floorMat.baseColour[0]);
	ImGui::Image((ImTextureID)(intptr_t)floorMat.baseMap->GetID(), ImVec2(100, 100));
	ImGui::SameLine();ImGui::Text("Main Texture");
	ImGui::Image((ImTextureID)(intptr_t)floorMat.normalMap->GetID(), ImVec2(100, 100));
	ImGui::SameLine();ImGui::Text("Normal Map");
	ImGui::Image((ImTextureID)(intptr_t)floorMat.parallaxMap->GetID(), ImVec2(100, 100));
	ImGui::SameLine(); ImGui::Text("Parallax/Height Map");
	ImGui::Checkbox("Use Parallax", &floorMat.useParallax);
	ImGui::SliderFloat("Parallax/Height Scale", &floorMat.heightScale, 0.0f, 0.08f);
	ImGui::SliderInt("Shinness", &floorMat.shinness, 32, 256);
	ImGui::End();


	ImGui::Begin("Experiment with Transforms");

	glm::vec3 translate, euler, scale;
	
	MathsHelper::DecomposeTransform(planeWorldTran, translate, euler, scale);

	glm::vec3 prev_rot = euler;
	bool update = ImGui::DragFloat3("Translate", &translate[0], 0.2f);
	update |= ImGui::DragFloat3("Rotation", &euler[0], 0.2f);
	update |= ImGui::DragFloat3("Scale", &scale[0], 0.2f);

	if (update)
	{
		planeWorldTran = glm::mat4(1.0f);
		
		glm::quat quat = glm::quat(glm::radians(euler));
		glm::mat4 rot = glm::toMat4(quat);
		planeWorldTran = glm::translate(planeWorldTran, translate) * rot * glm::scale(planeWorldTran, scale);
	}

	ImGui::End();
}

void ParallaxExperimentalScene::OnDestroy()
{
	//if (brickTex)
	//	brickTex->Clear();

	//if (brickNorMap)
	//	brickNorMap->Clear();

	if (floorMat.baseMap)
		floorMat.baseMap->Clear();
	if (floorMat.normalMap)
		floorMat.normalMap->Clear();

	Scene::OnDestroy();
}

void ParallaxExperimentalScene::CreateObjects()
{
	ground.Create();


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
	//brick texture 
	//brickTex = new Texture(FilePaths::Instance().GetPath("floor-brick-diff")/*, TextureFormat::SRGBA*/);
	//brickNorMap = new Texture(FilePaths::Instance().GetPath("floor-brick-nor"));
	
	//brickTex = new Texture(FilePaths::Instance().GetPath("brickwall-diff")/*, TextureFormat::SRGBA*/);
	//brickNorMap = new Texture(FilePaths::Instance().GetPath("brickwall-nor"));
	floorMat.name = "Floor Mat";

	if (false)
	{
		floorMat.baseMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("para-brick-diff"));
		floorMat.normalMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("para-brick-nor"));
		floorMat.parallaxMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("para-brick-disp"));
	}
	else
	{
		floorMat.baseMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("cobblestone-diff"));
		floorMat.normalMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("cobblestone-nor"));
		floorMat.parallaxMap = std::make_shared<Texture>(FilePaths::Instance().GetPath("cobblestone-disp"));
	}



	////////////////////////////////////////
// UNIFORM BUFFERs
////////////////////////////////////////
//------------------Camera Matrix Data UBO-----------------------------/
	long long int buf_size = 2 * sizeof(glm::mat4);// +sizeof(glm::vec2);   //to store view, projection
	m_CamMatUBO.Generate(buf_size);
	m_CamMatUBO.BindBufferRndIdx(0, buf_size, 0);
	modelShader.Bind();
	//i think what happens here is that the shader program 
	//scan/check its itself(program) for the block "u_CameraMat"
	//woudl be success if found
	modelShader.SetUniformBlockIdx("u_CameraMat", 0);
	//------------------Light Data UBO-----------------------------/
	//struct
	//DirectionalLight dirLight;
	//PointLight pointLights[MAX_POINT_LIGHTS];
	long long int light_buffer_size = 0;
	light_buffer_size += DirectionalLight::GetGPUSize();		//get the size of a directional light
	int pt_light_count = 1; 
	light_buffer_size += pt_light_count * PointLight::GetGPUSize();				//get the size of a point light

	m_LightDataUBO.Generate(light_buffer_size);
	m_LightDataUBO.BindBufferRndIdx(1, light_buffer_size, 0);
	modelShader.Bind();
	modelShader.SetUniformBlockIdx("u_LightBuffer", 1);



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
	dl.enable = true;
	dl.direction = glm::vec3(-1.0f, 1.0f, -1.0f);

	//Point Light
	ptLight.position = glm::vec3(0.0f, 5.0f, 0.0f);
	ptLight.colour = glm::vec3(0.0f, 1.0f, 0.0f);
	ptLight.ambientIntensity = 0.05f;
	ptLight.diffuseIntensity = 0.4f;
	ptLight.specularIntensity = 0.6f;
	ptLight.enable = true;


	ptLight.attenuation[0] = 1.0f; //constant
	ptLight.attenuation[1] = 0.07f; //linear
	ptLight.attenuation[2] = 0.017f; //quadratic



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

	//ground 
	model = glm::translate(model, groundPos);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f) * groundScale);
	shader.SetUniformMat4f("u_Model", model);

	//ground material
	if (apply_tex)
	{
		unsigned int tex_units = 0;
		shader.SetUniformVec3("u_Material.baseColour", floorMat.baseColour);
		if (floorMat.baseMap)
		{
			floorMat.baseMap->Activate(tex_units);
			shader.SetUniform1i("u_Material.baseMap", tex_units++);
		}
		if (floorMat.normalMap)
		{
			floorMat.normalMap->Activate(tex_units);
			shader.SetUniform1i("u_Material.normalMap", tex_units++);
		}
		if (floorMat.parallaxMap)
		{
			floorMat.parallaxMap->Activate(tex_units);
			shader.SetUniform1i("u_Material.parallaxMap", tex_units++);
		}
		shader.SetUniform1i("u_UseNorMap", useNor);
		shader.SetUniform1i("u_Material.shinness", floorMat.shinness);
		shader.SetUniform1i("u_Material.useParallax", floorMat.useParallax);
		shader.SetUniform1f("u_Material.parallax", floorMat.heightScale);
	}
	ground.Render();


	//draw plane 
	shader.SetUniformMat4f("u_Model", planeWorldTran);
	ground.Render();




	if (apply_tex)
	{
		shader.SetUniform1i("u_UseNorMap", 0);
		floorMat.baseMap->DisActivate(); //this should unbind all textures

	}

	shader.UnBind();
}

void ParallaxExperimentalScene::LightPass()
{
	//------------------Update uniform GPU buffer Lights -----------------------------/ 
	long long int offset_pointer = 0;
	offset_pointer = 0;
	dirLightObject.dirlight.UpdateUniformBufferData(m_LightDataUBO, offset_pointer);
	ptLight.UpdateUniformBufferData(m_LightDataUBO, offset_pointer);

}

void ParallaxExperimentalScene::ShadowPass()
{
	glCullFace(GL_FRONT);
	//--------------------Dir Light------------/
	shadowDepthShader.Bind();
	shadowDepthShader.SetUniform1i("u_IsOmnidir", 0);
	shadowDepthShader.SetUniformMat4f("u_LightSpaceMat", dirLightObject.dirLightShadow.GetLightSpaceMatrix());
	dirDepthMap.Write();
	glClear(GL_DEPTH_BUFFER_BIT);
	//SCENES/OBJECT TO RENDER
	DrawObjects(shadowDepthShader);
	dirDepthMap.UnBind();
	//--------------------Point Light---------------/
	shadowDepthShader.Bind();
	std::vector<glm::mat4> shadowMats = PointLightSpaceMatrix(ptLight.position, ptShadowConfig);
	//general shadowing values
	shadowDepthShader.SetUniform1i("u_IsOmnidir", 1);
	shadowDepthShader.SetUniform1f("u_FarPlane", ptShadowConfig.cam_far);
	shadowDepthShader.Bind();
	shadowDepthShader.SetUniformVec3("u_LightPos", ptLight.position);
	for (int f = 0; f < 6; ++f)
	{
		shadowDepthShader.SetUniformMat4f(("u_ShadowMatrices[" + std::to_string(f) + "]").c_str(), shadowMats[f]);
	}
	ptDepthCube.Write();//ready to write in the depth cube framebuffer for light "i"
	glClear(GL_DEPTH_BUFFER_BIT); //clear the depth buffer 
	DrawObjects(shadowDepthShader);
	ptDepthCube.UnBind();

	glCullFace(GL_BACK);
	glViewport(0, 0, window->GetWidth(), window->GetHeight()); //reset the view back just in case

}
