#include "GeometryScene.h"

#include "External Libs/imgui/imgui.h"


void GeometryScene::OnInit(Window* window)
{
	Scene::OnInit(window);

	window->UpdateProgramTitle("Geometry Scene");


	//Clear colour should start with magenta for debugging
	//m_ClearScreenColour = glm::vec3(1.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	if (!m_Camera)
		m_Camera = new Camera(glm::vec3(0.0f, /*5.0f*/7.0f, -36.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, 0.0f, 35.0f, 1.0f/*0.5f*/);

	CreateObjects();
}

void GeometryScene::OnUpdate(float delta_time)
{
	//Update stuffs..............

	//then render the stuffs and may be other stuffs..........................
	OnRender();
}

void GeometryScene::OnRender()
{
	//TO-DO: might re-define this if i want to run multiple scene at the same time
	glClearColor(m_ClearScreenColour.r, m_ClearScreenColour.g, m_ClearScreenColour.b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //Not using the stencil buffer now


	///////////////////////////////////////////
	// FIRST RENDER PASS
	///////////////////////////////////////////

	//Camera GPU buffer update
	glm::mat4 view = m_Camera->CalViewMat();
	glm::mat4 proj = m_Camera->CalculateProjMatrix(window->GetAspectRatio());
	m_CameraMatUBO.Bind();
	m_CameraMatUBO.SetBufferSubData(0, sizeof(glm::mat4), &proj[0][0]);
	m_CameraMatUBO.SetBufferSubData(sizeof(glm::mat4), sizeof(glm::mat4), &view[0][0]);
	m_CameraMatUBO.UnBind();

	//Model set-up
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(ground_rot), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, ground_scale);
	m_Shader.Bind();
	m_SquareMesh.GetVAO()->Bind();//shader would CRASHES BECAUSE SHADER NEEDS VERTEX DATA TO BE BINDED
	//m_SquareMesh.GetIBO()->Bind();
	m_Shader.SetUniformMat4f("u_Model", model); 
	m_Shader.SetUniform1f("u_Length", length);
	m_Shader.SetUniform1i("u_Debug", debugSquare);
	//m_SquareMesh.Render();
	//glDrawArrays(GL_POINTS, 0, 6);
	//glDrawArrays(GL_TRIANGLES, m_SquareMesh.GetIBO()->GetCount(), 4);
	//glDrawElements(GL_TRIANGLES, m_SquareMesh.GetIBO()->GetCount(), GL_UNSIGNED_INT, (void*)0);
	m_Shader.UnBind();


	//////////////////////////////
	// GROUND
	//////////////////////////////
	model = glm::mat4(1.0f);
	model = glm::translate(model, ground_pos);
	model = glm::rotate(model, glm::radians(ground_rot), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, ground_scale);
	m_GroundShader.Bind();
	m_GroundTex->Activate();
	m_GroundShader.SetUniformMat4f("u_projection", proj);
	m_GroundShader.SetUniformMat4f("u_view", view);
	m_GroundShader.SetUniformMat4f("u_model", model);
	m_GroundShader.SetUniform1i("u_DoDepthTest", 0.0f);
	m_GroundShader.SetUniform1f("u_Intensity", 1.0f);
	m_SquareMesh.Render();
	m_GroundTex->DisActivate();

	//RENDER SPHERE MAKE USE OF GROUND SHADER
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
	m_GroundShader.SetUniformMat4f("u_model", model);
	sphereTex->Activate();
	sphere.Render();

	//TEST CUBE WITH SPHERE PROP
	glm::mat4 test_cube_model = glm::mat4(1.0f);
	test_cube_model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
	m_GroundShader.SetUniformMat4f("u_model", test_cube_model);
	testCube.Render();


	////////////////
	// SECOND PASS ON SPHERE FOR DEBUGGING ITS NORMAL
	////////////////
	sphereNormDebugShader.Bind();
	sphereNormDebugShader.SetUniform1f("u_NorDebugLength", normDebugLength);
	sphereNormDebugShader.SetUniformVec3("u_DebugColour", normDebugColour);
	sphereNormDebugShader.SetUniform1i("u_UseDebugColour", useDebugColour);
	sphereNormDebugShader.SetUniformMat4f("u_Model", model);
	sphere.Render();

	//TEST CUBE WITH SPHERE PROP
	sphereNormDebugShader.SetUniformMat4f("u_Model", test_cube_model);
	testCube.Render();

	m_GroundShader.UnBind();
}

void GeometryScene::OnRenderUI()
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

	ImGui::SeparatorText("Scene Properties");
	ImGui::ColorEdit3("clear Screen Colour", &m_ClearScreenColour[0]);
	ImGui::SliderFloat("Length", &length, 0.0f, 10.0f, "%.2f");
	ImGui::Checkbox("Debug Square", &debugSquare);

	ImGui::Spacing();

	ImGui::DragFloat3("ground pos", &ground_pos[0], 0.1f);
	ImGui::DragFloat("ground rot", &ground_rot, 0.1f);
	ImGui::DragFloat3("ground scale", &ground_scale[0], 0.1f);

	ImGui::Spacing();

	ImGui::SeparatorText("Sphere normal debugging");
	ImGui::Checkbox("Use debug colour", &useDebugColour);
	ImGui::ColorEdit3("Debug colour", &normDebugColour[0]);
	ImGui::SliderFloat("Debug length", &normDebugLength, -1.0f, 2.0f, "%.2f");


	ImGui::End();

}

void GeometryScene::OnDestroy()
{
}

GeometryScene::~GeometryScene()
{
}

void GeometryScene::CreateObjects()
{

	///////////////////////////////////
	// CREATE SHADER
	///////////////////////////////////

	ShaderFilePath shader_file_path
					{
						"src/ShaderFiles/Learning/Geometry/LineVertex.glsl", //vertex shader
						"src/ShaderFiles/Learning/Geometry/LineFragment.glsl", //fragment shader
						"src/ShaderFiles/Learning/Geometry/LineGeometry.glsl"  //geometry shader
					};

	m_Shader.Create("shader_1", shader_file_path);


	////////////////////////////////////////
	// CREATE SQUARE MESH
	////////////////////////////////////////
	m_SquareMesh.Create();


	////////////////////////////////////////
	// CREATE CAMERA MAT UNIFORM BUFFER
	////////////////////////////////////////
	long long int buf_size = 2 * sizeof(glm::mat4);   //to store both view & projection
	m_CameraMatUBO.Generate(buf_size);



	////////////////////////////////////////
	// CREATE GROUND PROPERTIES
	////////////////////////////////////////
	ShaderFilePath ground_shader_file
				{
					"src/ShaderFiles/VertexLearningOpen.glsl",
					"src/ShaderFiles/Learning/DepthTestFrag.glsl" 
				};
	m_GroundShader.Create("ground_shader", ground_shader_file);

	m_GroundTex = new Texture("Assets/Textures/plain64.png");

	ground_scale = glm::vec3(5.0f);



	////////////////////////////////////////
	// CREATE SPHERE MESH
	////////////////////////////////////////
	sphere.Create();
	sphereTex = new Texture("Assets/Textures/brick.png");

	//SPHERE NORMAL DEBUG SHADER
	ShaderFilePath debug_sphere_shader_file
				{
					"src/ShaderFiles/Learning/Geometry/VertexDebugNormal.glsl", //vertex shader
					"src/ShaderFiles/Learning/Geometry/LineFragment.glsl", //frag shader
					"src/ShaderFiles/Learning/Geometry/GeometryDebugNormal.glsl"  //geometry shader
				};

	sphereNormDebugShader.Create("debug_norm_shader", debug_sphere_shader_file);


	////////////////////////////////////////
	// CREATE CUBE MESH
	////////////////////////////////////////
	testCube.Create();
}
