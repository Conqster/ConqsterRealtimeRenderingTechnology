#include "LearningRenderer.h"

#include "Core/Camera.h"
#include <iostream>

#include "Meshes/CubeMesh.h"
#include "Texture.h"

LearningRenderer::LearningRenderer()
				: RendererBase()
{
	m_SceneName = "Learning Scene";
	//CreateObject();
}

LearningRenderer::LearningRenderer(Window& use_window)
	: RendererBase(use_window)
{
	m_SceneName = "Learning Scene";
	CreateObject();
}

void LearningRenderer::Render(Camera& camera)
{
	m_MainShaderProgram.UseShader();
	glBindVertexArray(VAO);
	m_MainShaderProgram.SetUniformMat4f("u_view", camera.CalViewMat());
	m_MainShaderProgram.SetUniformVec3("u_ViewPos", camera.GetPosition());

	m_MainShaderProgram.SetUniformVec3("u_LightColour", test_LightColour);
	//m_MainShaderProgram.SetUniformVec3("u_ToyColour", test_ToyColour);


	////Test triangle
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(5.0f));
	//m_MainShaderProgram.SetUniformMat4f("u_model", model);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawArrays(GL_LINE_LOOP, 0, 3);




	//Test cube 
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f));
	m_MainShaderProgram.SetUniformMat4f("u_model", model);
	m_MainShaderProgram.SetUniformVec3("u_light.position", test_Light.position);
	m_MainShaderProgram.SetUniform1f("u_GlobalAmbientStrength", test_GlobalAmbientStrength);
	m_MainShaderProgram.SetUniform1f("u_GlobalDiffuseStrength", test_GlobalDiffuseStrength);
	m_MainShaderProgram.SetUniform1f("u_GlobalSpecularStrength", test_GlobalSpecularStrength);
	//m_MainShaderProgram.SetUniform1i("u_SpecularPower", test_SpecularPower);
	//Cube material 
	//int diffuseMap;
	//int specularMap;
	//int emissionMap;
	///*float*/ int shiniess;

	////stored as int foe simplity
	//bool invertSpecularMap;
	//glm::vec3 emissionColour;
	//float emissionStrength;
	//bool emit;
	m_MainShaderProgram.SetUniform1i("u_Material.diffuseMap", test_Material.diffuseMap);
	m_MainShaderProgram.SetUniform1i("u_Material.specularMap", test_Material.specularMap);
	m_MainShaderProgram.SetUniform1i("u_Material.emissionMap", test_Material.emissionMap);
	m_MainShaderProgram.SetUniform1i("u_Material.shininess", test_Material.shiniess);
	m_MainShaderProgram.SetUniform1i("u_Material.invertSpecularMap", test_Material.invertSpecularMap);
	m_MainShaderProgram.SetUniformVec3("u_Material.emissionColour", test_Material.emissionColour);
	m_MainShaderProgram.SetUniform1f("u_Material.emissionMapStrength", test_Material.emissionStrength);
	m_MainShaderProgram.SetUniform1i("u_Material.emit", test_Material.emit);
	m_MainShaderProgram.SetUniform1i("u_IsLight", 0);
	// render the cube
	cube_DiffuseMap_Test->Activate(test_Material.diffuseMap);
	cube_SpecularMap_Test->Activate(test_Material.specularMap);
	cube_EmissionMap_Test->Activate(test_Material.emissionMap);
	//cube_Test->Render();
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	cube_DiffuseMap_Test->DisActivate();
	cube_SpecularMap_Test->DisActivate();
	cube_EmissionMap_Test->DisActivate();




	//Test cube as light
	model = glm::mat4(1.0f);
	model = glm::translate(model, test_Light.position);
	model = glm::scale(model, glm::vec3(1.2f));
	m_MainShaderProgram.SetUniformMat4f("u_model", model);
	m_MainShaderProgram.SetUniformVec3("u_light.ambient", test_Light.ambient);
	m_MainShaderProgram.SetUniformVec3("u_light.diffuse", test_Light.diffuse);
	m_MainShaderProgram.SetUniformVec3("u_light.specular", test_Light.specular);
	m_MainShaderProgram.SetUniform1i("u_IsLight", 1);
	cube_Test->Render();



	glBindVertexArray(0);
	//std::cout << "Trying to render stuffs!!!!!!!!!! \n";
}

void LearningRenderer::CreateObject()
{

	cube_Test = new CubeMesh();
	cube_Test->Create();

	//static Texture new_texture("Assets/Textures/container.pn
	// g");
	//cube_AmbientMap_Test = &new_texture;

	cube_DiffuseMap_Test = new Texture("Assets/Textures/container.png");
	cube_SpecularMap_Test = new Texture("Assets/Textures/container_specular.png");
	cube_EmissionMap_Test = new Texture("Assets/Textures/container_emit_matrix.jpg");
	//texture_Test->LoadTexture("Assets/Textures/brick.png");

	float vertices[] = 
	{
		 0.0f,  0.5f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f,
	};




	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 4));



	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////

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
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

}
