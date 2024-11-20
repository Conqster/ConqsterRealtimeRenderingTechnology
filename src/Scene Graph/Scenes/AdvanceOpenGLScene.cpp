#include "AdvanceOpenGLScene.h"

#include "Renderer/Renderer.h"
#include "External Libs/imgui/imgui.h"

#include "Renderer/Meshes/Mesh.h"

#include "Scene Graph/Scenes/SceneManager.h"
#include "Renderer/Meshes/PrimitiveMeshFactory.h"

void AdvanceOpenGLScene::OnInit(Window* window)
{
	Scene::OnInit(window);

	window->UpdateProgramTitle("Advance openGL Scene");

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 20.0f, 1.0f/*0.5f*/);

	CreateObjects();
}

void AdvanceOpenGLScene::OnUpdate(float delta_time)
{
	OnRender();
}

void AdvanceOpenGLScene::OnRender()
{
	//glEnable(GL_DEPTH_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	grassShader.Bind();
	grassShader.SetUniformMat4f("u_Projection", m_Camera->CalculateProjMatrix(window->GetAspectRatio()));
	grassShader.SetUniformMat4f("u_View", m_Camera->CalViewMat());


	outlineShader.Bind();
	outlineShader.SetUniformMat4f("u_Projection", m_Camera->CalculateProjMatrix(window->GetAspectRatio()));
	outlineShader.SetUniformMat4f("u_View", m_Camera->CalViewMat());

	objectShaders.Bind();
	objectShaders.SetUniformMat4f("u_Projection", m_Camera->CalculateProjMatrix(window->GetAspectRatio()));
	objectShaders.SetUniformMat4f("u_View", m_Camera->CalViewMat());
	objectShaders.SetUniform1f("u_Near", *m_Camera->Ptr_Near());
	objectShaders.SetUniform1f("u_Intensity", brightness);
	objectShaders.SetUniform1i("u_DoDepthTest", doDepthTest);
	//objectShaders.SetUniformVec3("u_ViewPos", m_Camera->GetPosition());


	// draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
	glStencilMask(0x00);
	marbleTex->Activate(0);
	glBindVertexArray(m_Plane.VAO);
	objectShaders.SetUniformMat4f("u_Model", glm::mat4(1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 6);




	// 1st. render pass, draw objects as normal, writing to the stencil buffer
	// --------------------------------------------------------------------
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	glm::mat4 model = glm::mat4(1.0f);

	glBindVertexArray(m_Cube.VAO);
	metalTex->Activate(0);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0));
	objectShaders.SetUniformMat4f("u_Model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f,0.0));
	objectShaders.SetUniformMat4f("u_Model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);



	if (secondpass_stencil)
	{
		// 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
		// Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
		// the objects' size differences, making it look like borders.
		// -----------------------------------------------------------------------------------------------------------------------------
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0xFF);
		glDisable(GL_DEPTH_TEST);
		outlineShader.Bind();
		outlineShader.SetUniformVec4("u_stencilColour", outlineColour);
		glBindVertexArray(m_Cube.VAO);
		metalTex->Activate(0);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0));
		model = glm::scale(model, glm::vec3(1.0f) * outlineThickness);
		outlineShader.SetUniformMat4f("u_Model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0));
		model = glm::scale(model, glm::vec3(1.0f) * outlineThickness);
		outlineShader.SetUniformMat4f("u_Model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);

	}


	grassShader.Bind();
	grassTexture->Activate();
	glm::mat4 model_g = glm::mat4(1.0f);
	model_g = glm::translate(model_g, grassPos);
	model_g = glm::rotate(model_g, glm::radians(grassRot), grassRotAxis);
	grassShader.SetUniformMat4f("u_Model", model_g);
	//grassMesh->Render();
	m_SceneRenderer.DrawMesh(*grassMesh);
}

void AdvanceOpenGLScene::OnRenderUI()
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

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SliderFloat("Brightness", &brightness, 0.1f, 3.0f);
	ImGui::Spacing();
	ImGui::Checkbox("Depth Test", &doDepthTest);
	ImGui::Checkbox("Stencil Pass", &secondpass_stencil);
	ImGui::ColorEdit4("Stencil Colour", &outlineColour[0]);
	ImGui::SliderFloat("Stencil Thinkness", &outlineThickness, 0.1f, 10.0f);

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::SliderFloat3("Grass Rot Axis", &grassRotAxis[0], 0.0f, 1.0f);
	ImGui::DragFloat3("Grass Position", &grassPos[0], 0.1f);
	ImGui::SliderFloat("Grass Rot", &grassRot, 0.0f, 180.0f);



	ImGui::End();
}

void AdvanceOpenGLScene::OnDestroy()
{

	marbleTex->UnRegisterUse();
	delete marbleTex;
	marbleTex = nullptr;

	metalTex->UnRegisterUse();
	delete metalTex;
	metalTex = nullptr;


	GLCall(glDeleteBuffers(1, &m_Cube.VBO));
	GLCall(glDeleteVertexArrays(1, &m_Cube.VAO));
	GLCall(glDeleteBuffers(1, &m_Plane.VBO));
	GLCall(glDeleteVertexArrays(1, &m_Plane.VAO));

	objectShaders.Clear();
	outlineShader.Clear();

	grassMesh->Clear();
	grassShader.Clear();

	Scene::OnDestroy();
}

AdvanceOpenGLScene::~AdvanceOpenGLScene()
{
}

void AdvanceOpenGLScene::CreateObjects()
{
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);



	///////////////////////////////////////////////////////////////////////////////////////
	////////////////////// PLANE
	///////////////////////////////////////////////////////////////////////////////////////

	float plane_vertices[] = {

		5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
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


	metalTex = new Texture("Assets/Textures/metal.jpeg");
	marbleTex = new Texture("Assets/Textures/marble.jpeg");

	ShaderFilePath file_path{ "Assets/Shaders/Learning/DepthTestVertex.glsl", "Assets/Shaders/Learning/DepthTestFrag.glsl" };
	objectShaders.Create("depth_test_learning", file_path);

	
	ShaderFilePath outlineShader_file_path{ "Assets/Shaders/Learning/DepthTestVertex.glsl", "Assets/Shaders/Learning/OutlineFragStencil.glsl" };
	outlineShader.Create("stencil_outline_test", outlineShader_file_path);


	grassMesh = CRRT::PrimitiveMeshFactory::Instance().CreateQuad();

	grassTexture = new Texture("Assets/Textures/blending_transparent_window.png");

	ShaderFilePath grass_file_path{ "Assets/Shaders/Learning/DepthTestVertex.glsl", "Assets/Shaders/Learning/GrassFragShader.glsl" };
	grassShader.Create("grass_shader", grass_file_path);
}
 
//
//static void RegisterScene(const char* name)
//{
//	SceneManager::Instance().RegisterScene(name, []() -> Scene*
//		{
//			return new AdvanceOpenGLScene();
//		});
//}