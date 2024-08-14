#include "Textures_FrameBufferScene.h"

#include "External Libs/imgui/imgui.h"

#include "Graphics/Texture.h"
#include "Graphics/TextureCube.h"
#include <iostream>

#include "External Libs/stb_image/stb_image.h"

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
	//glViewport(0, 0, window->GetWidth(), window->GetHeight());
	//glViewport(500, 500, window->GetWidth() * 0.5f, window->GetHeight() * 0.5f);
	glViewport(0, 0, window->GetWidth(), window->GetHeight());
	/////////////////////////////////////////////////////////////////////	
	// First Pass
	/////////////////////////////////////////////////////////////////////	
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glEnable(GL_DEPTH_TEST);
	//TO-DO: might re-define this if i want to run multiple scene at the same time
	glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 0.0f);
	//glClear(GL_COLOR_BUFFER_BIT);   //Alway need to clear buffers 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //Not using the stencil buffer now
	DrawObjects(/*true*/false, true); //true to do depth test on the object,


	/////////////////////////////////////////////////////////////////////	
	// Second Pass
	/////////////////////////////////////////////////////////////////////	
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // return back to default buffer
	glClearColor(m_ClearColourSecondPass.r, m_ClearColourSecondPass.g, m_ClearColourSecondPass.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	DrawObjects(false, false);
	

	uint16_t win_width = window->GetWidth() * 0.25f; // 0.25f;
	uint16_t win_height = window->GetHeight() * 0.25f; // 0.25f;
	uint16_t x_offset = win_width * 0.6f,
			y_offset = win_height * 0.6f;
	uint16_t x_pos = window->GetWidth() - (win_width * 0.5f) - x_offset;
	uint16_t y_pos = window->GetHeight() - (win_height * 0.5f) - y_offset;

	glViewport(x_pos, y_pos, win_width, win_height);
	screenShader.Bind();
	glBindVertexArray(m_Quad.VAO);
	glBindTexture(GL_TEXTURE_2D, fboTex);
	screenShader.SetUniform1f("u_Offset", screenTexSampleOffset);
	screenShader.SetUniform1i("u_DoSpecial", specialScreenKernel);
	screenShader.SetUniform1i("u_KernelType", specialKernelType);
	screenShader.SetUniformVec3("u_TexColour", m_PlayColourFBOTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glDrawArrays(GL_LINE_LOOP, 0, 6);

	//glDisable(GL_DEPTH_TEST);
	/////////////////////////////////////////////////////////////////////	
	// Test new pass for 
	/////////////////////////////////////////////////////////////////////	
	//x_pos = window->GetWidth() - (win_width * 2.0f) - (x_offset);
	//glViewport(x_pos, y_pos, win_width, win_height);
	//DrawObjects(false, false);
	// 
	//
	//screenShader.Bind();
	//glBindVertexArray(m_Quad.VAO);
	//glBindTexture(GL_TEXTURE_2D, fboTex);
	//screenShader.SetUniform1f("u_Offset", screenTexSampleOffset);
	//screenShader.SetUniform1i("u_DoSpecial", specialScreenKernel);
	//screenShader.SetUniform1i("u_KernelType", specialKernelType);
	//screenShader.SetUniformVec3("u_TexColour", m_PlayColourFBOTexture);
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	/////////////////////////////////////////////////////////////////////	
	// Test new pass
	/////////////////////////////////////////////////////////////////////	
	// 
	//TO-DO: might re-define this if i want to run multiple scene at the same time
	//glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 0.0f);
	//glClear(GL_COLOR_BUFFER_BIT);   //Alway need to clear buffers 
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //Not using the stencil buffer now
	//glEnable(GL_DEPTH_TEST);


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

	ImGui::ColorEdit3("texture colour", &m_PlayColourFBOTexture[0]);

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



	ImGui::End();
}

void Texture_FrameBufferScene::OnDestroy()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &FBO);

	m_PlaneTex->UnRegisterUse();
	delete m_PlaneTex;
	m_PlaneTex = nullptr;

	m_CrateTex->UnRegisterUse();
	delete m_CrateTex;
	m_CrateTex = nullptr;

	glDeleteVertexArrays(1, &m_Quad.VAO);
	glDeleteVertexArrays(1, &m_Cube.VAO);
	glDeleteVertexArrays(1, &m_Plane.VAO);

	glDeleteBuffers(1, &m_Quad.VBO);
	glDeleteBuffers(1, &m_Cube.VBO);
	glDeleteBuffers(1, &m_Plane.VBO);

	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &fboTex);        //delte frame texture/ texture attached to frame buffer


	glDeleteRenderbuffers(1, &RBO);

	/////////////////
	// Sky box
	/////////////////
	m_SkyboxMap->UnRegisterUse();
	delete m_SkyboxMap;
	m_SkyboxMap = nullptr;

	glDeleteVertexArrays(1, &m_Skybox_vertex.VAO);
	glDeleteBuffers(1, &m_Skybox_vertex.VBO);

	Scene::OnDestroy();
}

Texture_FrameBufferScene::~Texture_FrameBufferScene()
{


}

void Texture_FrameBufferScene::CreateObjects()
{
	std::vector<std::string> skybox_faces
	{
		"Assets/Textures/Skybox/right.jpg",
		"Assets/Textures/Skybox/left.jpg",
		"Assets/Textures/Skybox/top.jpg",
		"Assets/Textures/Skybox/bottom.jpg",
		"Assets/Textures/Skybox/front.jpg",
		"Assets/Textures/Skybox/back.jpg"
	};

	///////////////////////////////////////////////////////////////////////
	// NEW NEW SKY BOX: Cube Texture Map
	///////////////////////////////////////////////////////////////////////
	m_DefaultSkybox.Create(skybox_faces);

	///////////////////////////////////////////////////////////////////////
	// SKY BOX: Cube Texture Map
	///////////////////////////////////////////////////////////////////////
	m_SkyboxMap = new TextureCube(skybox_faces);

	ShaderFilePath skybox_shader_file_path
	{ "src/ShaderFiles/SkyboxVertex.glsl",
		//"src/ShaderFiles/Learning/ScreenFrameFrag.glsl" };
		"src/ShaderFiles/SkyboxFragment.glsl" };
	m_SkyboxShader.Create("skybox_shader", skybox_shader_file_path);



	///////////////////////////////////////////////////////////////////////
	// SKY BOX: VAO & VBO
	///////////////////////////////////////////////////////////////////////
	float skybox_vertices[] = {

		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f

	};


	glGenVertexArrays(1, &m_Skybox_vertex.VAO);
	glBindVertexArray(m_Skybox_vertex.VAO);

	glGenBuffers(1, &m_Skybox_vertex.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_Skybox_vertex.VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);

	//Only the pos, is required no need for the UV coord 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(skybox_vertices[0]) * 3, (void*)0);
	
	glBindVertexArray(0);


	///////////////////////////////////////////////////////////////////////
	// Frame Buffer creation
	///////////////////////////////////////////////////////////////////////
	uint16_t use_width = window->GetWidth(); // 2.0f;
	uint16_t use_height = window->GetHeight(); // 2.0f;

	//glViewport(0, 0, window->GetWidth() * 0.5f, window->GetHeight() * 0.5f);
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	///////////////////////////////////////////////////////////////////////
	// create colour attachment texture for frame buffer
	///////////////////////////////////////////////////////////////////////
	glGenTextures(1, &fboTex);
	glBindTexture(GL_TEXTURE_2D, fboTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, use_width, use_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//attach this new texture(fboTex) to the framebuffer FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex, 0);


	///////////////////////////////////////////////////////////////////////
	// create a render buffer object for depth and stencil 
	///////////////////////////////////////////////////////////////////////
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, use_width, use_height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "[FRAMEBUFFER ERROR]: Framebuffer did not complete!!!!\n";

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//glViewport(0,0, window->GetWidth(), window->GetHeight());

	///////////////////////////////////////////////////////////////////////
	// Create Quad Object vertex
	///////////////////////////////////////////////////////////////////////

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
					{ "src/ShaderFiles/Learning/ScreenFrameVertex.glsl",
					//"src/ShaderFiles/Learning/ScreenFrameFrag.glsl" };
					"src/ShaderFiles/Learning/KernelFragTextureEffect.glsl" };
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
	m_CrateTex = new Texture("Assets/Textures/container2.jpg");

	ShaderFilePath file_path
					{ "src/ShaderFiles/VertexLearningOpen.glsl",
					//"src/ShaderFiles/Learning/GrassFragShader.glsl" };
					"src/ShaderFiles/Learning/DepthTestFrag.glsl" };
	m_CrateShader.Create("shader_1", file_path);


	m_CrateShader.Bind();
	m_CrateShader.SetUniform1f("u_Intensity", 1.0f);
	//m_CrateShader.SetUniform1i("u_DoDepthTest", 0);

	/////////////////////////////
	// Object sampler texture
	/////////////////////////////

	ShaderFilePath sample_skybox_file_path
					{ "src/ShaderFiles/Learning/ObjSampleSkyboxVer.glsl",
					"src/ShaderFiles/Learning/ObjSampleSkyboxFrag.glsl" };
	m_ObjectSampleReflect.Create("shader_obj_sampler", sample_skybox_file_path);

	////////////////////////////////////
	// Create Spheres
	////////////////////////////////////
	m_Sphere.Create();

	uint16_t num_of_sphere = 5;
	glm::vec3 sphere_pos = glm::vec3(0.0f);
	glm::vec3 offset = glm::vec3(1.5f, 0.0f, 0.0f);
	glm::vec3 sphere_scale = glm::vec3(1.0f);
	float scale_offset = 1.0f;
	glm::vec3 liftoff_ground = glm::vec3(0.0f, 1.0f, 0.0f);
	float scale_weight = 0;
	for (uint16_t i = 0; i < num_of_sphere; i++)
	{
		Transform new_sphere_trans;
		new_sphere_trans.scale = sphere_scale + glm::vec3(1 * i * scale_offset);
		scale_weight = new_sphere_trans.scale.x;
		new_sphere_trans.pos = sphere_pos + (liftoff_ground * scale_weight) + (scale_weight * glm::vec3(1.0f, 0.0f, 0.0f) * (offset * (float)i));
		m_SceneSphereTranforms.push_back(new_sphere_trans);
	}


	///////////////////////////////////////////////////////////////////////////////////////
	////////////////////// PLANE
	///////////////////////////////////////////////////////////////////////////////////////

	float plane_vertices[] = {

		5.0f, 0.0f,5.0f,  2.0f, 0.0f,
		-5.0f,0.0f, 5.0f,  0.0f, 0.0f,
		-5.0f,0.0f,-5.0f,  0.0f, 2.0f,
			 //0.0f,
		 5.0f,0.0f, 5.0f,  2.0f, 0.0f,
		-5.0f,0.0f,-5.0f,  0.0f, 2.0f,
		 5.0f,0.0f,-5.0f,  2.0f, 2.0f
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


	m_PlaneTex = new Texture("Assets/Textures/marble.jpeg");
	//marbleTex = new Texture("Assets/Textures/marble.jpeg");
}

void Texture_FrameBufferScene::DrawObjects(bool depth_test, bool use_rear)
{
	glm::mat4 rear_view = glm::mat4(1.0f);
	if (use_rear)
	{
		glm::vec3 Cpos = m_Camera->GetPosition();
		glm::vec3 CFor = m_Camera->GetFroward();
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


	m_ObjectSampleReflect.SetUniformVec3("u_CamPos", m_Camera->GetPosition());
	m_SkyboxMap->Activate();

	m_ObjectSampleReflect.SetUniformMat4f("u_Model", sample_model);
	glBindVertexArray(m_Cube.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);


	////Object 2. Multiple Sphere
	for (size_t i = 0; i < m_SceneSphereTranforms.size(); i++)
	{
		sample_model = glm::mat4(1.0f);
		sample_model = glm::translate(sample_model, m_SceneSphereTranforms[i].pos);
		sample_model = glm::scale(sample_model, m_SceneSphereTranforms[i].scale);
		m_ObjectSampleReflect.SetUniformMat4f("u_Model", sample_model);
		m_Sphere.Render();

	}
	//sample_model = glm::translate(sample_model, glm::vec3(20.0f, 0.0f, 0.0f));
	//sample_model = glm::translate(sample_model, m_SphereTrans.pos + m_SphereTrans.scale);






	///////////////////////////////////////////////////////////
	// DRAW Sky box
	///////////////////////////////////////////////////////////
	if (use_rear)
	{
		Camera new_camera = *m_Camera;
		*(new_camera.New_Yaw()) += 180.0f;
		m_DefaultSkybox.Draw(new_camera, *window);
	}
	else
		m_DefaultSkybox.Draw(*m_Camera, *window);



	//m_SkyboxShader.Bind();
	//glDepthMask(GL_FALSE);

	//glm::mat4 sky_view = glm::mat4(glm::mat3((use_rear) ? rear_view : m_Camera->CalViewMat()));
	////glm::mat4 sky_view = glm::mat4(glm::mat3(m_Camera->CalViewMat()));
	//m_SkyboxShader.SetUniformMat4f("u_View", sky_view);
	//m_SkyboxShader.SetUniformMat4f("u_Projection", m_Camera->CalculateProjMatrix(window->GetAspectRatio()));

	//glBindVertexArray(m_Skybox_vertex.VAO);
	//m_SkyboxMap->Activate();
	////glm::mat4 model = glm::mat4(1.0f);
	////glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glDepthMask(GL_TRUE);
	//glBindVertexArray(0);
	//m_SkyboxShader.UnBind();
}
