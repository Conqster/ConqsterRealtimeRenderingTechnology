#include "Textures_FrameBufferScene.h"

#include "External Libs/imgui/imgui.h"
#include <iostream>

#include "External Libs/stb_image/stb_image.h"

#include "EventHandle.h"
#include "SceneManager.h"

#include "Renderer/Meshes/PrimitiveMeshFactory.h"



void Texture_FrameBufferScene::OnInit(Window* window)
{
	Scene::OnInit(window);

	window->UpdateProgramTitle("Texture & Frame Buffer Scene");

	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glDepthFunc(GL_LEQUAL);  //less eqaul but of skybox as doing depth test as part of the skybox fails due to been obscurely by other object 
							 // causing the triangle corresponding to that obscure pixel to jitter. 
	//glEnable(GL_CULL_FACE);


	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 35.0f, 1.0f/*0.5f*/);

	CreateObjects();
}

void Texture_FrameBufferScene::OnUpdate(float delta_time)
{
	//Update stuffs..............

	//then render the stuffs and may be other stuffs..........................
	OnRender();
}

void Texture_FrameBufferScene::OnRender()
{
	
	/////////////////////////////////////////////////////////////////////	
	// First Pass
	/////////////////////////////////////////////////////////////////////	
	m_Framebuffer.Bind();
	glEnable(GL_DEPTH_TEST);
	//TO-DO: might re-define this if i want to run multiple scene at the same time
	glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //Not using the stencil buffer now
	DrawObjects(m_PerfromDepthTest, m_FrameCaptureRear); //true to do depth test on the object,


	/////////////////////////////////////////////////////////////////////	
	// Second Pass
	/////////////////////////////////////////////////////////////////////	
	m_Framebuffer.UnBind(); // return back to default buffer
	glViewport(0, 0, window->GetWidth(), window->GetHeight());
	glClearColor(m_ClearColourSecondPass.r, m_ClearColourSecondPass.g, m_ClearColourSecondPass.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	DrawObjects(false, false);
	

	float win_width = (float)window->GetWidth() * 0.25f; // 0.25f;
	float win_height = (float)window->GetHeight() * 0.25f; // 0.25f;
	float x_offset = win_width * 0.6f,
			y_offset = win_height * 0.6f;
	float x_pos = (float)window->GetWidth() - (win_width * 0.5f) - x_offset;
	float y_pos = (float)window->GetHeight() - (win_height * 0.5f) - y_offset;

	glViewport((int)x_pos, (int)y_pos, (uint16_t)win_width, (uint16_t)win_height);
	screenShader.Bind();
	glBindVertexArray(m_Quad.VAO);

	//use texture render onto from framebuffer first pass
	m_Framebuffer.BindTexture();

	screenShader.SetUniform1f("u_Offset", screenTexSampleOffset);
	screenShader.SetUniform1i("u_DoSpecial", specialScreenKernel);
	screenShader.SetUniform1i("u_KernelType", specialKernelType);
	screenShader.SetUniformVec3("u_TexColour", m_PlayColourFBOTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

}

void Texture_FrameBufferScene::OnRenderUI()
{

	ImGui::Begin("Debug Texture FrameBuffer");

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

	//MOUSE PROP
	ImGui::Text("Mouse Position, X: %f, Y: %f", EventHandle::MousePosition().x, EventHandle::MousePosition().y);


	if (ImGui::TreeNode("Camera Properties"))
	{
		ImGui::SliderFloat("Move Speed", m_Camera->Ptr_MoveSpeed(), 5.0f, 50.0f);
		ImGui::SliderFloat("Rot Speed", m_Camera->Ptr_RotSpeed(), 0.0f, 2.0f);

		float window_width = (float)window->GetWidth();
		float window_height = (float)window->GetHeight();
		static glm::mat4 test_proj;

		bool update_camera_proj = false;

		update_camera_proj = ImGui::SliderFloat("FOV", m_Camera->Ptr_FOV(), 0.0f, 179.0f, "%.1f");
		update_camera_proj |= ImGui::DragFloat("Near", m_Camera->Ptr_Near(), 0.1f, 0.1f, 50.0f, "%.1f");
		update_camera_proj |= ImGui::DragFloat("Far", m_Camera->Ptr_Far(), 0.1f, 0.0f, 500.0f, "%.1f");

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
	ImGui::ColorEdit3("clear Second Pass Colour", &m_ClearColourSecondPass[0]);


	ImGui::Spacing();
	ImGui::SliderFloat("Object Sample skybox influencity", &m_SkyboxInfluencity, 0.0f, 1.0f, "%.1f");
	ImGui::ColorEdit3("Object Texture Colour", &m_TextureColour[0]);
	ImGui::Spacing();

	ImGui::ColorEdit3("texture colour", &m_PlayColourFBOTexture[0]);

	ImGui::SeparatorText("Frame buffer Properties");
	ImGui::Checkbox("Capture Rear", &m_FrameCaptureRear);
	ImGui::Checkbox("Depth test", &m_PerfromDepthTest);

	ImGui::SeparatorText("Kernel Effect");
	ImGui::SliderFloat("texture sample offset", &screenTexSampleOffset, 50.0f, 500.0f, "%.1f");
	ImGui::Checkbox("perform special kernels", &specialScreenKernel);
	ImGui::Text("'0' for Blur effect and '1' for for edge decteion effect!!!");
	ImGui::InputInt("Effect type", &specialKernelType);

	/////////////////////////////////////////////////////////////////////
	// CUBE PROPERTIES
	/////////////////////////////////////////////////////////////////////
	ImGui::SeparatorText("Cube Properties");
	ImGui::DragFloat3("Cube Position", &m_CubeWorTrans.pos.x, 0.1f);
	ImGui::SliderFloat3("Cube Scale", &m_CubeWorTrans.scale.x, 0.1f, 10.0f, "%.1f");

	/////////////////////////////////////////////////////////////////////
	// PLANE PROPERTIES
	/////////////////////////////////////////////////////////////////////
	ImGui::SeparatorText("Plane Properties");
	ImGui::DragFloat3("Plane Position", &m_PlaneWorTrans.pos.x, 0.1f);
	ImGui::SliderFloat3("Plane Scale", &m_PlaneWorTrans.scale.x, 0.1f, 10.0f, "%.1f");

	ImGui::SeparatorText("Scene Spheres Properties");

	for (size_t i = 0; i < m_SceneSphereTranforms.size(); i++)
	{
		ImGui::PushID(&m_SceneSphereTranforms[i]);
		std::string idx = std::to_string(i);
		ImGui::Text("Sphere index %s", idx.c_str());
		ImGui::DragFloat3("Sphere Position", &m_SceneSphereTranforms[i].pos.x, 0.1f);
		ImGui::SliderFloat3("Sphere Scale", &m_SceneSphereTranforms[i].scale.x, 0.1f, 10.0f, "%.1f");
		ImGui::PopID();
	}

	ImGui::SeparatorText("Stats");
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	//ImGui::Text("Game Time: %f", del);



	ImGui::End();
}

void Texture_FrameBufferScene::OnDestroy()
{
	m_PlaneTex->UnRegisterUse();
	delete m_PlaneTex;
	m_PlaneTex = nullptr;

	m_CrateTex->UnRegisterUse();
	delete m_CrateTex;
	m_CrateTex = nullptr;

	m_SphereTex->UnRegisterUse();
	delete m_SphereTex;
	m_SphereTex = nullptr;

	glDeleteVertexArrays(1, &m_Quad.VAO);
	glDeleteVertexArrays(1, &m_Cube.VAO);
	glDeleteVertexArrays(1, &m_Plane.VAO);

	glDeleteBuffers(1, &m_Quad.VBO);
	glDeleteBuffers(1, &m_Cube.VBO);
	glDeleteBuffers(1, &m_Plane.VBO);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//m_Framebuffer.UnBind();
	m_Framebuffer.Delete();
	Scene::OnDestroy();
}

Texture_FrameBufferScene::~Texture_FrameBufferScene()
{

}

void Texture_FrameBufferScene::CreateObjects()
{
	///////////////////////////////////////////////////////////////////////
	// SKY BOX: Cube Texture Map
	///////////////////////////////////////////////////////////////////////
	std::vector<std::string> def_skybox_faces
	{
		"Assets/Textures/Skyboxes/default_skybox/right.jpg",
		"Assets/Textures/Skyboxes/default_skybox/left.jpg",
		"Assets/Textures/Skyboxes/default_skybox/top.jpg",
		"Assets/Textures/Skyboxes/default_skybox/bottom.jpg",
		"Assets/Textures/Skyboxes/default_skybox/front.jpg",
		"Assets/Textures/Skyboxes/default_skybox/back.jpg"
	};

	std::vector<std::string> skybox_faces_2
	{
		"Assets/Textures/Skyboxes/envmap_stormydays/right.tga",
		"Assets/Textures/Skyboxes/envmap_stormydays/left.tga",
		"Assets/Textures/Skyboxes/envmap_stormydays/top.tga",
		"Assets/Textures/Skyboxes/envmap_stormydays/bottom.tga",
		"Assets/Textures/Skyboxes/envmap_stormydays/front.tga",
		"Assets/Textures/Skyboxes/envmap_stormydays/back.tga"
	};

	m_DefaultSkybox.Create(def_skybox_faces);
	//m_DefaultSkybox.Create(skybox_faces_2);

	
	///////////////////////////////////////////////////////////////////////
	// Frame Buffer creation
	///////////////////////////////////////////////////////////////////////
	uint16_t use_width = window->GetWidth(); // 2.0f;
	uint16_t use_height = window->GetHeight(); // 2.0f;
	m_Framebuffer.Generate(use_width, use_height);

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


	///////////////////////////////////////////////////////////////////////
	// Quad Screen shader 
	///////////////////////////////////////////////////////////////////////
	ShaderFilePath screen_file_path
					{ "Assets/Shaders/Learning/ScreenFrameVertex.glsl",
					//"Assets/Shaders/Learning/ScreenFrameFrag.glsl" };
					"Assets/Shaders/Learning/KernelFragTextureEffect.glsl" };
	screenShader.Create("screen_shader", screen_file_path);


	screenShader.Bind();
	screenShader.SetUniform1i("u_ScreenTex", 0);



	float cube_vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	// first, configure the cube's VAO (and VBO)
	glGenVertexArrays(1, &m_Cube.VAO);
	glGenBuffers(1, &m_Cube.VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_Cube.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_Cube.VAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);



	//m_CrateTex = new Texture("Assets/Textures/container.png");
	//m_CrateTex = new Texture("Assets/Textures/brick.png");
	m_CrateTex = new Texture("Assets/Textures/At Manchester.jpg");

	ShaderFilePath file_path
					{ "Assets/Shaders/VertexLearningOpen.glsl",
					//"Assets/Shaders/Learning/GrassFragShader.glsl" };
					"Assets/Shaders/Learning/DepthTestFrag.glsl" };
	m_CrateShader.Create("shader_1", file_path);


	m_CrateShader.Bind();
	m_CrateShader.SetUniform1f("u_Intensity", 1.0f);
	//m_CrateShader.SetUniform1i("u_DoDepthTest", 0);

	/////////////////////////////
	// Object sampler texture
	/////////////////////////////

	ShaderFilePath sample_skybox_file_path
					{ "Assets/Shaders/Learning/ObjSampleSkyboxVer.glsl",
					"Assets/Shaders/Learning/ObjSampleSkyboxFrag.glsl" };
	m_ObjectSampleReflect.Create("shader_obj_sampler", sample_skybox_file_path);

	////////////////////////////////////
	// Create Spheres
	////////////////////////////////////
	m_Sphere = CRRT::PrimitiveMeshFactory::Instance().CreateASphere();
	m_SphereTex = new Texture("Assets/Textures/plain64.png");

	uint16_t num_of_sphere = 10;
	glm::vec3 sphere_pos = glm::vec3(-20.0f, 1.0f, 0.0f);
	float offset = 6.0f;
	glm::vec3 sphere_scale = glm::vec3(1.5f);

	glm::vec3 new_offset = glm::vec3(0.0f);
	for (uint16_t i = 0; i < num_of_sphere; i++)
	{
		new_offset.x = (float)i * offset;

		for (uint16_t j = 0; j < (num_of_sphere / 2); j++)
		{
			new_offset.y = (float)j * offset;

			for (uint16_t k = 0; k < 3; k++)
			{
				new_offset.z = (float)k * offset;

				Transform new_sphere_trans;
				new_sphere_trans.scale = sphere_scale;
				new_sphere_trans.pos = sphere_pos + new_offset;
				m_SceneSphereTranforms.push_back(new_sphere_trans);
			}
		}
	}


	///////////////////////////////////////////////////////////////////////////////////////
	////////////////////// PLANE
	///////////////////////////////////////////////////////////////////////////////////////

	float plane_vertices[] = {

		5.0f, 0.0f,5.0f,  4.0f, 0.0f,
		-5.0f,0.0f, 5.0f,  0.0f, 0.0f,
		-5.0f,0.0f,-5.0f,  0.0f, 4.0f,
			 //0.0f,
		 5.0f,0.0f, 5.0f,  4.0f, 0.0f,
		-5.0f,0.0f,-5.0f,  0.0f, 4.0f,
		 5.0f,0.0f,-5.0f,  4.0f, 4.0f
	};

	glGenVertexArrays(1, &m_Plane.VAO);
	glGenBuffers(1, &m_Plane.VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_Plane.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices, GL_STATIC_DRAW);
	glBindVertexArray(m_Plane.VAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(plane_vertices[0]), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(plane_vertices[0]), (void*)(3 * sizeof(plane_vertices[0])));

	m_PlaneWorTrans.pos = glm::vec3(0.0f, -0.5f, 0.0f);
	m_PlaneWorTrans.scale = glm::vec3(5.0f, 0.0f, 5.0f);


	//m_PlaneTex = new Texture("Assets/Textures/dirt.png");
	m_PlaneTex = new Texture("Assets/Textures/concrete_panels_4k.gltf/textures/concrete_panels_diff_4k.jpg");
	//marbleTex = new Texture("Assets/Textures/marble.jpeg");
}

void Texture_FrameBufferScene::DrawObjects(bool depth_test, bool use_rear)
{
	glm::mat4 rear_view = glm::mat4(1.0f);
	if (use_rear)
	{
		glm::vec3 Cpos = m_Camera->GetPosition();
		glm::vec3 CFor = m_Camera->GetForward();
		glm::vec3 CUp = m_Camera->GetUp();

		rear_view = glm::lookAt(Cpos, Cpos - CFor, CUp);
	}

	///////////////////////////////////////////////////////////
	// DRAW MAIN SCENE
	///////////////////////////////////////////////////////////


	m_CrateShader.Bind();
	m_CrateShader.SetUniform1i("u_DoDepthTest", depth_test);
	m_CrateShader.SetUniformMat4f("u_view", m_Camera->CalViewMat());
	m_CrateShader.SetUniformMat4f("u_projection", m_Camera->CalculateProjMatrix(window->GetAspectRatio()));

	if (use_rear)
		m_CrateShader.SetUniformMat4f("u_view", rear_view);

	if (depth_test)
		m_CrateShader.SetUniform1f("u_Near", *m_Camera->Ptr_Near());

	glm::mat4 model = glm::mat4(1.0f);

	/////////////////////////////////////////////////////////////////////	
	// CUBE PROPERTIES	
	/////////////////////////////////////////////////////////////////////
	glBindVertexArray(m_Cube.VAO);
	model = glm::translate(model, m_CubeWorTrans.pos);
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, m_CubeWorTrans.scale);


	m_CrateTex->Activate();

	//if(depth_test)
	//	m_CrateTex->Activate();
	//else
	//	glBindTexture(GL_TEXTURE_2D, fboTex);

	m_CrateShader.SetUniformMat4f("u_model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	


	/////////////////////////////////////////////////////////////////////
	// CUBE TWO
	/////////////////////////////////////////////////////////////////////
	//model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(5.0f, 0.0f, 5.0f));
	m_CrateShader.SetUniformMat4f("u_model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	/////////////////////////////////////////////////////////////////////
	// CUBE THREE
	/////////////////////////////////////////////////////////////////////
	model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 0.0f));
	m_CrateShader.SetUniformMat4f("u_model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);

	/////////////////////////////////////////////////////////////////////
	// PLANE PROPERTIES
	/////////////////////////////////////////////////////////////////////
	glBindVertexArray(m_Plane.VAO);
	model = glm::mat4(1.0f);
	model = glm::translate(model, m_PlaneWorTrans.pos);
	model = glm::scale(model, m_PlaneWorTrans.scale);

	m_PlaneTex->Activate();

	//if(depth_test)
	//	m_PlaneTex->Activate();
	//else
	//	glBindTexture(GL_TEXTURE_2D, fboTex);

	//Since shader has not been unbind its fine
	m_CrateShader.SetUniformMat4f("u_model", model);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);



	///////////////////////////////////////////////////////////
	// Play with testing Object sampler
	///////////////////////////////////////////////////////////
	m_ObjectSampleReflect.Bind();
	glm::mat4 sample_model = glm::mat4(1.0f);
	//sample_model = glm::scale(sample_model, glm::vec3(2.0f));
	sample_model = glm::translate(sample_model, glm::vec3(5.0f, 0.0f, 0.0f));

	m_ObjectSampleReflect.SetUniformMat4f("u_View", m_Camera->CalViewMat());
	m_ObjectSampleReflect.SetUniformMat4f("u_Projection", m_Camera->CalculateProjMatrix(window->GetAspectRatio()));

	m_ObjectSampleReflect.SetUniform1i("u_DoDepthTest", depth_test);
	if (depth_test)
		m_ObjectSampleReflect.SetUniform1f("u_Near", *m_Camera->Ptr_Near());


	m_ObjectSampleReflect.SetUniformVec3("u_CamPos", m_Camera->GetPosition());
	m_DefaultSkybox.ActivateMap();

	m_ObjectSampleReflect.SetUniformMat4f("u_Model", sample_model);
	glBindVertexArray(m_Cube.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);


	////Object 2. Multiple Sphere
	//Sphere texture 
	m_SphereTex->Activate(1);
	m_ObjectSampleReflect.SetUniform1f("u_SkyboxInfluencity", m_SkyboxInfluencity);
	m_ObjectSampleReflect.SetUniformVec3("u_TextureColour", m_TextureColour);
	for (size_t i = 0; i < m_SceneSphereTranforms.size(); i++)
	{
		sample_model = glm::mat4(1.0f);
		sample_model = glm::translate(sample_model, m_SceneSphereTranforms[i].pos);
		sample_model = glm::scale(sample_model, m_SceneSphereTranforms[i].scale);
		m_ObjectSampleReflect.SetUniformMat4f("u_Model", sample_model);
		//m_Sphere.Render();
		m_SceneRenderer.DrawMesh(m_Sphere);
	}


	///////////////////////////////////////////////////////////
	// DRAW Sky box
	///////////////////////////////////////////////////////////

	if (depth_test)
		return;

	if (use_rear)
	{
		Camera new_camera = *m_Camera;
		*(new_camera.New_Yaw()) += 180.0f;
		m_DefaultSkybox.Draw(new_camera, *window);
	}
	else
		m_DefaultSkybox.Draw(*m_Camera, *window);

}

//
//void Texture_FrameBufferScene::RegisterScene(const char* name)
//{
//	SceneManager::Instance().RegisterScene(name, []() -> Scene*
//		{
//			return new Texture_FrameBufferScene();
//		});
//
//	std::cout << "register Texture_FrameBufferScene!!!!!!!!!!!!\n";
//}