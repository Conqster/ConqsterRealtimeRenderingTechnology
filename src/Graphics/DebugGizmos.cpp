#include "DebugGizmos.h"

#include "RendererErrorAssertion.h"
#include <glm/gtc/matrix_transform.hpp>



bool DebugGizmos::active = false;
Shader DebugGizmos::m_Shader;
SphereMesh DebugGizmos::sphere;
//UniformBuffer* DebugGizmos::m_CameraMatUBO = nullptr;


DebugGizmos::~DebugGizmos()
{
	//clean up

}




void DebugGizmos::Startup()
{
	Generate();
}

//Make sure Camera data uniform buffer is generated & update d
void DebugGizmos::DrawLine(glm::vec3 v1, glm::vec3 v2, glm::vec3 colour, float thickness)
{
	if (!active)
		Generate();


	m_Shader.Bind();
	glm::mat4 model = glm::mat4(1.0f);
	m_Shader.SetUniformMat4f("u_Model", model);
	m_Shader.SetUniformVec3("u_Colour", colour);
	GLCall(glLineWidth(thickness));
	glBegin(GL_LINES);
	glVertex3fv(&v1[0]); 
	glVertex3fv(&v2[0]); 	
	glEnd();
	glLineWidth(1.0f);
	m_Shader.UnBind();
}

void DebugGizmos::DrawWireSphere(glm::vec3 p, float radius, glm::vec3 colour, float thickness)
{
	if (!active)
		Generate();

	m_Shader.Bind();
	glm::mat4 model = glm::mat4(1.0f);			
	model = glm::translate(model, p);
	model = glm::scale(model, glm::vec3(radius));
	m_Shader.SetUniformMat4f("u_Model", model);
	m_Shader.SetUniformVec3("u_Colour", colour);
	sphere.RenderDebugOutLine(thickness);
	m_Shader.UnBind();
}

void DebugGizmos::DrawSphere(glm::vec3 p, float radius, glm::vec3 colour)
{
	if (!active)
		Generate();

	m_Shader.Bind();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, p);
	model = glm::scale(model, glm::vec3(radius));
	m_Shader.SetUniformMat4f("u_Model", model);
	m_Shader.SetUniformVec3("u_Colour", colour);
	sphere.Render();
	m_Shader.UnBind();
}

void DebugGizmos::Cleanup()
{
	//m_CameraMatUBO->Delete();
	m_Shader.Clear();
}

void DebugGizmos::Generate()
{
	ShaderFilePath debug_shader_file_path
	{
		"src/ShaderFiles/Learning/Debugger/PrimitiveVertex.glsl", //vertex shader
		"src/ShaderFiles/Learning/Debugger/PrimitiveFrag.glsl", //fragment shader
	};
	m_Shader.Create("debug_shader", debug_shader_file_path);

	sphere.Create();

	active = true;

}
