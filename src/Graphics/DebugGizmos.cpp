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

void DebugGizmos::DrawSquare(glm::vec3 center, glm::vec3 forward, float left, float right, float bottom, float top, glm::vec3 colour, float thickness)
{
	if (!active)
		Generate();

	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_Model", glm::mat4(1.0f));
	m_Shader.SetUniformVec3("u_Colour", colour);

	//v1/////////v2//
	//############///
	//#//////////#///
	//#//////////#///
	//############///
	//v3/////////v4//

	//glm::vec3 rightV = glm::normalize(glm::cross(p, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 rightV = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 0.0f, 1.0f)));
	//glm::vec3 upV = glm::normalize(glm::cross(rightV, dir));
	glm::vec3 upV = glm::normalize(glm::cross(rightV, forward));

	glm::vec3 v1 = center + (upV * top) + (rightV * left);
	glm::vec3 v2 = center + (upV * top) + (rightV * right);
	glm::vec3 v3 = center + (upV * bottom) + (rightV * right);
	glm::vec3 v4 = center + (upV * bottom) + (rightV * left);


	GLCall(glLineWidth(thickness));
	glBegin(GL_LINE_LOOP);
	glVertex3fv(&v1[0]);
	glVertex3fv(&v2[0]);
	glVertex3fv(&v3[0]);
	glVertex3fv(&v4[0]);
	glEnd();
	glLineWidth(1.0f);
	m_Shader.UnBind();
}

void DebugGizmos::DrawOrthoCameraFrustrm(glm::vec3 pos, glm::vec3 forward, float cam_near, float cam_far, float left, float right, float bottom, float top, glm::vec3 colour, float thickness)
{
	if (!active)
		Generate();

	glm::vec3 rightV = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 0.0f, 1.0f)));
	glm::vec3 upV = glm::normalize(glm::cross(rightV, forward));

	glm::vec3 boundsTL = (upV * top) + (rightV * left);
	glm::vec3 boundsTR = (upV * top) + (rightV * right);
	glm::vec3 boundsBL = (upV * bottom) + (rightV * left);
	glm::vec3 boundsBR = (upV * bottom) + (rightV * right);

	glm::vec3 nearPlane = pos + (glm::normalize(forward) * cam_near);

	glm::vec3 nearTL = nearPlane + boundsTL;
	glm::vec3 nearTR = nearPlane + boundsTR;
	glm::vec3 nearBL = nearPlane + boundsBL;
	glm::vec3 nearBR = nearPlane + boundsBR;

	glm::vec3 farPlane = pos + (glm::normalize(-forward) * cam_far);


	glm::vec3 farTR = farPlane + boundsTL;
	glm::vec3 farTL = farPlane + boundsTR;
	glm::vec3 farBR = farPlane + boundsBL;
	glm::vec3 farBL = farPlane + boundsBR;


	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_Model", glm::mat4(1.0f));
	m_Shader.SetUniformVec3("u_Colour", colour);

	GLCall(glLineWidth(thickness));
	glBegin(GL_LINE_LOOP);
	//near plane bounds vertex draw
	glVertex3fv(&nearTL[0]);
	glVertex3fv(&nearTR[0]);
	glVertex3fv(&nearBR[0]);
	glVertex3fv(&nearBL[0]);
	//far plane bounds vertex draw
	glVertex3fv(&farBR[0]);
	glVertex3fv(&farBL[0]);
	glVertex3fv(&farTL[0]);
	glVertex3fv(&farTR[0]);
	glEnd();


	glBegin(GL_LINES);
	glVertex3fv(&nearTL[0]);
	glVertex3fv(&nearBL[0]);


	glVertex3fv(&farBR[0]);
	glVertex3fv(&farTR[0]);

	glVertex3fv(&nearBR[0]);
	glVertex3fv(&farBL[0]);

	glVertex3fv(&nearTR[0]);
	glVertex3fv(&farTL[0]);

	glVertex3fv(&nearPlane[0]);
	glVertex3fv(&glm::vec3(nearPlane - (forward * 4.0f))[0]);
	glEnd();

	glLineWidth(1.0f);
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

	//sphere.Create();
	sphere.Create(12);

	active = true;

}
