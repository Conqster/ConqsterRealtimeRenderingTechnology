#include "FaceCullingScene.h"

#include "Graphics/RendererErrorAssertion.h"
#include "External Libs/imgui/imgui.h"

#include "Graphics/Texture.h"
#include <iostream>

void FaceCullingScene::OnInit(Window* window)
{
	Scene::OnInit(window);

	window->UpdateProgramTitle("Face Culling Scene");

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_PROGRAM_POINT_SIZE); 

	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 1.0f/*0.5f*/);

	CreateObjects();
}

void FaceCullingScene::OnUpdate(float delta_time)
{
	//Update stuffs..............

	//then render the stuffs and may be other stuffs..........................
	OnRender();
}

void FaceCullingScene::OnRender()
{

	glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_PROGRAM_POINT_SIZE);
	m_DefaultShader.Bind();
	//shader.SetUniformMat4f("u_Projection", m_Camera->CalculateProjMatrix(window->GetAspectRatio()));
	//shader.SetUniformMat4f("u_View", m_Camera->CalViewMat());

	
	///////////////////////////////////////////////
	// bind and assign Uniform Buffer UBO
	///////////////////////////////////////////////
	glm::mat4 proj = m_Camera->CalculateProjMatrix(window->GetAspectRatio());
	glm::mat4 view = m_Camera->CalViewMat();

	//glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &proj[0][0]);
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view[0][0]);
	m_CameraMatUBO.Bind();
	m_CameraMatUBO.SetBufferSubData(0, sizeof(glm::mat4), &proj[0][0]);
	m_CameraMatUBO.SetBufferSubData(sizeof(glm::mat4), sizeof(glm::mat4), &view[0][0]);
	m_CameraMatUBO.UnBind();


	glm::mat4 model = glm::mat4(1.0f);

	glBindVertexArray(m_Cube.VAO);
	crateTex->Activate(0);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0));
	m_DefaultShader.SetUniformMat4f("u_Model", model);
	//shader.SetUniform1i("u_DoDepthTest", 0);
	//shader.SetUniform1f("u_Intensity", 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	m_Obj1Shader.Bind();
	model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
	m_Obj1Shader.SetUniformMat4f("u_Model", model);
	//shader.SetUniform1i("u_UsePointSize", 1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//glDrawArrays(GL_POINTS, 0, 36);
	//shader.SetUniform1i("u_UsePointSize", 0);

	///////////////
	// Extra Objs 
	///////////////
	m_Obj2Shader.Bind();
	model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
	m_Obj2Shader.SetUniformMat4f("u_Model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	m_Obj3Shader.Bind();
	model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
	m_Obj3Shader.SetUniformMat4f("u_Model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

}

void FaceCullingScene::OnRenderUI()
{
	ImGui::Begin("Debug Advance OpenGL");

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





	ImGui::End();
}

void FaceCullingScene::OnDestroy()
{
	crateTex->UnRegisterUse();
	delete crateTex;
	crateTex = nullptr;

	GLCall(glDeleteVertexArrays(1, &m_Cube.VAO));
	GLCall(glDeleteBuffers(1, &m_Cube.VBO));

	m_DefaultShader.Clear();
	m_Obj1Shader.Clear();
	m_Obj2Shader.Clear();
	m_Obj3Shader.Clear();

	m_CameraMatUBO.Delete();

	Scene::OnDestroy();

}

FaceCullingScene::~FaceCullingScene()
{
	std::cout << "[CLOSING]: Face-Culling Scene!!!!!!!!!!!!!!!!!!!!!!!!\n";
}

void FaceCullingScene::CreateObjects()
{
	float cube_vertices[] = {
		/* */	// positions			//colour					// normals				// texture coords
		/*1*/	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f, 	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,
		/*2*/	 1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f, 1.0f, 	0.0f,  0.0f, -1.0f,		1.0f, 0.0f,
		/*3*/	 1.0f,  1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f, 	0.0f,  0.0f, -1.0f,		1.0f, 1.0f,
		/*4*/	 1.0f,  1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f, 	0.0f,  0.0f, -1.0f,		1.0f, 1.0f,
		/*5*/	-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f, 1.0f, 	0.0f,  0.0f, -1.0f,		0.0f, 1.0f,
		/*6*/	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f, 	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,
			
		/*7*/	-1.0f, -1.0f,  1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		0.0f,  0.0f, 1.0f,		0.0f, 0.0f,
		/*8*/	 1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		0.0f,  0.0f, 1.0f,		1.0f, 0.0f,
		/*9*/	 1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		0.0f,  0.0f, 1.0f,		1.0f, 1.0f,
		/*10*/	 1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		0.0f,  0.0f, 1.0f,		1.0f, 1.0f,
		/*11*/	-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		0.0f,  0.0f, 1.0f,		0.0f, 1.0f,
		/*12*/	-1.0f, -1.0f,  1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		0.0f,  0.0f, 1.0f,		0.0f, 0.0f,
			
		/*13*/	-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		-1.0f,  0.0f,  0.0f,	 1.0f, 0.0f,
		/*14*/	-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		-1.0f,  0.0f,  0.0f,	 1.0f, 1.0f,
		/*15*/	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		-1.0f,  0.0f,  0.0f,	 0.0f, 1.0f,
		/*16*/	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		-1.0f,  0.0f,  0.0f,	 0.0f, 1.0f,
		/*17*/	-1.0f, -1.0f,  1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		-1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
		/*18*/	-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		-1.0f,  0.0f,  0.0f,	 1.0f, 0.0f,
		
		/*19*/	 1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		 1.0f,  0.0f,  0.0f,	 1.0f, 0.0f,
		/*20*/	 1.0f,  1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		 1.0f,  0.0f,  0.0f,	 1.0f, 1.0f,
		/*21*/	 1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		 1.0f,  0.0f,  0.0f,	 0.0f, 1.0f,
		/*22*/	 1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		 1.0f,  0.0f,  0.0f,	 0.0f, 1.0f,
		/*23*/	 1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		 1.0f,  0.0f,  0.0f,	 0.0f, 0.0f,
		/*24*/	 1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		 1.0f,  0.0f,  0.0f,	 1.0f, 0.0f,
	
		/*25*/	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		 0.0f, -1.0f,  0.0f,	 0.0f, 1.0f,
		/*26*/	 1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		 0.0f, -1.0f,  0.0f,	 1.0f, 1.0f,
		/*27*/	 1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		 0.0f, -1.0f,  0.0f,	 1.0f, 0.0f,
		/*28*/	 1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		 0.0f, -1.0f,  0.0f,	 1.0f, 0.0f,
		/*29*/	-1.0f, -1.0f,  1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		 0.0f, -1.0f,  0.0f,	 0.0f, 0.0f,
		/*30*/	-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		 0.0f, -1.0f,  0.0f,	 0.0f, 1.0f,
		
		/*31*/	-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		 0.0f,  1.0f,  0.0f,	 0.0f, 1.0f,
		/*32*/	 1.0f,  1.0f, -1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		 0.0f,  1.0f,  0.0f,	 1.0f, 1.0f,
		/*33*/	 1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		 0.0f,  1.0f,  0.0f,	 1.0f, 0.0f,
		/*34*/	 1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		 0.0f,  1.0f,  0.0f,	 1.0f, 0.0f,
		/*35*/	-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f, 1.0f,		 0.0f,  1.0f,  0.0f,	 0.0f, 0.0f,
		/*36*/	-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f, 1.0f,		 0.0f,  1.0f,  0.0f,	 0.0f, 1.0f
	};


	// first, configure the cube's VAO (and VBO)
	glGenVertexArrays(1, &m_Cube.VAO);
	glGenBuffers(1, &m_Cube.VBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_Cube.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindVertexArray(m_Cube.VAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//colour 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// normal attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(3);

	//uv attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(10 * sizeof(float)));
	glEnableVertexAttribArray(2);



	crateTex = new Texture("Assets/Textures/container.png");

	///////////////////////////////////////////////
	// Create Shaders
	///////////////////////////////////////////////

	ShaderFilePath file_path
		{ "src/ShaderFiles/Advance/Vertex.glsl", 
		"src/ShaderFiles/Advance/Fragment.glsl" };
	m_DefaultShader.Create("shader_1", file_path);

	ShaderFilePath file_path_2
					{ "src/ShaderFiles/Advance/Obj1_Vertex.glsl",
					"src/ShaderFiles/Advance/Fragment.glsl" };
	m_Obj1Shader.Create("shader_2", file_path_2);

	ShaderFilePath file_path_3
					{ "src/ShaderFiles/Advance/Obj2_Vertex.glsl",
					"src/ShaderFiles/Advance/Fragment.glsl" };
	m_Obj2Shader.Create("shader_3", file_path_3);

	ShaderFilePath file_path_4
					{ "src/ShaderFiles/Advance/Obj3_Vertex.glsl",
					"src/ShaderFiles/Advance/Fragment.glsl" };
	m_Obj3Shader.Create("shader_4", file_path_4);


	///////////////////////////////////////////////
	// Generate Uniform Buffer UBO
	///////////////////////////////////////////////

	//using "2 * sizeof(glm::mat4);" as this buffer will be used for storing/accessing projection & view matrics
	long long int buffer_size = 2 * sizeof(glm::mat4);

	m_CameraMatUBO.Generate(buffer_size);

}
