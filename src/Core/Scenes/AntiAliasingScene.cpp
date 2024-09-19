#include "AntiAliasingScene.h"

#include "Graphics/RendererErrorAssertion.h"

#include "External Libs/imgui/imgui.h"

void AntiAliasingScene::OnInit(Window* window)
{
	Scene::OnInit(window);

	window->UpdateProgramTitle("AntiAliasing Scene");
	

	////TESTING TEST
	//glfwWindowHint(GLFW_SAMPLES, 4);
	//glEnable(GL_MULTISAMPLE);
	
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 35.0f, 1.0f/*0.5f*/);

	CreateObjects();
}

void AntiAliasingScene::OnUpdate(float delta_time)
{
	//hack to keep track of window resize 
	static int _width;
	static int _height;

	if (_width != window->GetWidth() || _height != window->GetHeight())
	{
		_width = window->GetWidth();
		_height = window->GetHeight();
		m_MSAA.Resize(_width, _height);
		m_MSAA2.Resize(_width, _height);
	}


	OnRender();
}

void AntiAliasingScene::OnRender()
{
	//render
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//Camera prop
	glm::mat4 view = m_Camera->CalViewMat();
	glm::mat4 proj = m_Camera->CalculateProjMatrix(window->GetAspectRatio());

	//Cube shader prop & set trans 
	cubeShader.Bind();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(1.0f) * scaleBy);
	cubeShader.SetUniformMat4f("u_Projection", proj);
	cubeShader.SetUniformMat4f("u_View", view);
	cubeShader.SetUniformMat4f("u_Model", model);
	cubeShader.UnBind();

	/////////////////////////////////////////////
	// Normal Rendering without MSAA
	/////////////////////////////////////////////
	if (!m_DoMSAA)
	{
		cubeShader.Bind();
		cube.Render();
		cubeShader.UnBind();
		return;
	}


	/////////////////////////////////////////////
	// Do MSAA1 instead
	/////////////////////////////////////////////
	if (!m_DoMSAA2)
	{
		///////////////////////////////////////////
		// FIRST RENDER PASS: Draw scene as normal in MSAA buffers
		///////////////////////////////////////////
		//draw scene (as normal in MSAA buffer)
		m_MSAA.Bind();
		GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GLCall(glEnable(GL_DEPTH_TEST));


		//use shader, set trans & draw
		cubeShader.Bind();
		cube.Render();
		cubeShader.UnBind();

		///////////////////////////////////////////
		// Blit multisample buffers to normal colour buffer of MSAA FBO
		///////////////////////////////////////////
		//now blit MSAA buffer to normal colour of intermediate FBO
		m_MSAA.Blit();

		/////////////////////////////////////////// 
		// SECOND RENDER PASS: Render quad with scene visual as its texture image 
		///////////////////////////////////////////
		m_MSAA.UnBind();
		GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
		GLCall(glDisable(GL_DEPTH_TEST));

		//DRAW SCREEN QUAD by using texture from first pass
		screenShader.Bind();
		screenShader.SetUniform1i("u_Debug", m_DebugScreenTex);
		screenShader.SetUniformVec3("u_DebugColour", m_DebugScreenTexColour);
		GLCall(glBindVertexArray(m_Quad.VAO));
		m_MSAA.BindTexture(0);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
		m_MSAA.UnBindTexture();
		screenShader.UnBind();
		return;
	}



	/////////////////////////////////////////////
	// Do MSAA2 instead
	/////////////////////////////////////////////
	///////////////////////////////////////////
	// FIRST RENDER PASS: Draw scene as normal in MSAA buffers
	///////////////////////////////////////////
	//draw scene (as normal in MSAA buffer)
	m_MSAA2.Bind();
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GLCall(glEnable(GL_DEPTH_TEST));


	//use shader, set trans & draw
	cubeShader.Bind();
	cube.Render();
	cubeShader.UnBind();

	///////////////////////////////////////////
	// Blit multisample buffers to normal colour buffer of MSAA FBO
	///////////////////////////////////////////
	//now blit MSAA buffer to normal colour of intermediate FBO
	m_MSAA2.Blit();

	/////////////////////////////////////////// 
	// SECOND RENDER PASS: Render quad with scene visual as its texture image 
	///////////////////////////////////////////
	m_MSAA2.UnBind();
	GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
	GLCall(glDisable(GL_DEPTH_TEST));

	//DRAW SCREEN QUAD by using texture from first pass
	screenShader.Bind();
	screenShader.SetUniform1i("u_Debug", m_DebugScreenTex);
	screenShader.SetUniformVec3("u_DebugColour", m_DebugScreenTexColour);
	screenShader.SetUniform1i("u_ViewWidth", window->GetWidth());
	screenShader.SetUniform1i("u_ViewHeight", window->GetHeight());
	screenShader.SetUniform1i("u_SampleCount", m_SampleCount);
	screenShader.SetUniform1i("u_ScreenCaptureTex", 1);
	GLCall(glBindVertexArray(m_Quad.VAO));
	//m_MSAA2.BindTexture(0);
	m_MSAA2.BindTextureMultiSample(1);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	screenShader.SetUniform1i("u_ViewWidth", 0); //to fail (if-else check) in shader, as like a reset preventing other use of the shader
	screenShader.UnBind();
	m_MSAA2.UnBindTextureMS();

}

void AntiAliasingScene::OnRenderUI()
{
	ImGui::Begin("Debug AntiAliasing Scene");

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

	///////////////////////////////////
	// SCENE SPECIFIC
	///////////////////////////////////
	ImGui::Spacing();
	ImGui::SeparatorText("Scene Properties");
	ImGui::Checkbox("Do MSAA", &m_DoMSAA);
	if (m_DoMSAA)
	{
		ImGui::Checkbox("Do MSAA 2 instead", &m_DoMSAA2);
		ImGui::SliderInt("MSAA 2 Sample Count", &m_SampleCount, 1, 8);
	}
	ImGui::ColorEdit3("Clear Screen", &m_ClearScreenColour[0]);
	ImGui::Checkbox("Debug Scene", &useDebugColour);
	ImGui::ColorEdit3("Debug", &debugColour[0]);
	ImGui::SliderFloat("cube Scale", &scaleBy, 0.0f, 10.0f, "%.1f");
	
	ImGui::Spacing();
	ImGui::SeparatorText("Debug Screen Texture");
	ImGui::Checkbox("Debug Screen", &m_DebugScreenTex);
	ImGui::ColorEdit3("Screen Debug Colour", &m_DebugScreenTexColour[0]);

	ImGui::End();
}

void AntiAliasingScene::OnDestroy()
{
	cubeShader.Clear();
	cube.Clear();
	GLCall(glDeleteBuffers(1, &m_Quad.VBO));
	GLCall(glDeleteVertexArrays(1, &m_Quad.VAO));
	screenShader.Clear();
	m_MSAA.Delete();
	m_MSAA2.Delete();

	//GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	//GLCall(glBindVertexArray(0));

	Scene::OnDestroy();
}

AntiAliasingScene::~AntiAliasingScene()
{
}

void AntiAliasingScene::CreateObjects()
{

	////////////////////////////////////////
	// CREATE MSAA
	////////////////////////////////////////
	m_MSAA.Generate(window->GetWidth(), window->GetHeight(), 2);
	m_MSAA2.Generate(window->GetWidth(), window->GetHeight(), 8);


	///////////////////////////////////
	// CREATE SHADER
	///////////////////////////////////

	ShaderFilePath shader_file_path
	{
		"src/ShaderFiles/Learning/MSAA/Anti_Aliasing_Vertex.glsl", //vertex shader
		"src/ShaderFiles/Learning/MSAA/Anti_Aliasing_Frag.glsl", //fragment shader
	};

	cubeShader.Create("shader_1", shader_file_path);

	////////////////////////////////////////
	// CREATE SQUARE MESH
	////////////////////////////////////////
	cube.Create();


	float quad_vertices[] = {

		// x   y		 u	   v
		-1.0f,  1.0f,	0.0f, 1.0f,
		-1.0f, -1.0f,	0.0f, 0.0f,
		 1.0f, -1.0f,	1.0f, 0.0f,

		-1.0f,  1.0f,	0.0f, 1.0f,
		 1.0f, -1.0,	1.0f, 0.0f,
		 1.0f,  1.0,	1.0f, 1.0f
	};

	GLCall(glGenVertexArrays(1, &m_Quad.VAO));
	GLCall(glGenBuffers(1, &m_Quad.VBO));

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_Quad.VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW));

	GLCall(glBindVertexArray(m_Quad.VAO));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(quad_vertices[0]) * 4, (void*)0));

	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(quad_vertices[0]) * 4, (void*)(sizeof(quad_vertices[0]) * 2)));


	///////////////////////////////////////////////////////////////////////
	// Quad Screen shader 
	///////////////////////////////////////////////////////////////////////
	ShaderFilePath screen_shader_file_path
	{ 
		"src/ShaderFiles/Learning/MSAA/AA_PostProcess_Vertex.glsl",
		//"src/ShaderFiles/Learning/ScreenFrameFrag.glsl" };
		"src/ShaderFiles/Learning/MSAA/AA_PostProcess_Frag.glsl" 
	};
	screenShader.Create("screen_shader", screen_shader_file_path);


	//screenShader.Bind();
	//screenShader.SetUniform1i("u_ScreenTex", 0);
	//screenShader.UnBind();

}
