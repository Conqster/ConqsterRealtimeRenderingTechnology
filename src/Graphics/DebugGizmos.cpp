#include "DebugGizmos.h"

#include "RendererErrorAssertion.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Util/MathsHelpers.h"

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

void DebugGizmos::DrawRay(glm::vec3 v1, glm::vec3 dir, float dist, glm::vec3 colour, float thickness)
{
	DrawLine(v1, v1 + (dir * dist), colour, thickness);
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


void DebugGizmos::DrawBox(AABB aabb, glm::vec3 colour, float thickness)
{
	if (!active)
		Generate();

	
	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_Model", glm::mat4(1.0f));
	m_Shader.SetUniformVec3("u_Colour", colour);

	GLCall(glLineWidth(thickness));
	glBegin(GL_LINE_LOOP);

	//v1/////////v2//
	//############///
	//#//////////#///
	//#//////////#///
	//############///
	//v3/////////v4//
	//front box/square  v1>>v2>>v4>>v3
	glm::vec<3, double> a = aabb.m_Min;
	glm::vec<3, double> b = aabb.m_Max;
	glVertex3dv(&a[0]);
	glVertex3dv(&glm::vec<3, double>(b.x, a.y, a.z)[0]);
	glVertex3dv(&glm::vec<3, double>(b.x, b.y, a.z)[0]);
	glVertex3dv(&glm::vec<3, double>(a.x, b.y, a.z)[0]);
	//v3>>v7
	//v5/////////v6//
	//############///
	//#//////////#///
	//#//////////#///
	//############///
	//v7/////////v8//
	//back box/square v7>>v8>>v6>>v5
	glVertex3dv(&glm::vec<3, double>(a.x, b.y, b.z)[0]);
	glVertex3dv(&b[0]);
	glVertex3dv(&glm::vec<3, double>(b.x, a.y, b.z)[0]);
	glVertex3dv(&glm::vec<3, double>(a.x, a.y, b.z)[0]);
	glEnd();

	glBegin(GL_LINES);
	//front left v1>>v3
	glVertex3d(a.x, a.y, a.z);
	glVertex3d(a.x, b.y, a.z);

	//back left v5>>v7
	glVertex3d(a.x, a.y, b.z);
	glVertex3d(a.x, b.y, b.z);

	//front top >> back top v2>>v6
	glVertex3d(b.x, a.y, a.z);
	glVertex3d(b.x, a.y, b.z);

	//front bottom >> back bottom v4>>v8
	glVertex3d(b.x, b.y, a.z);
	glVertex3d(b.x, b.y, b.z);
	glEnd();

	glLineWidth(1.0f);
	m_Shader.UnBind();
}

void DebugGizmos::DrawCross(glm::vec3 center, float size, bool axis_colour, glm::vec3 colour, float thickness)
{
	float half_size = size * 0.5f;
	//front to back (Z axis) blue
	glm::vec3 _colour = (axis_colour) ? glm::vec3(0.0f, 0.0f, 1.0f) : colour;
	DrawLine(center + glm::vec3(0.0f, 0.0f, half_size), center - glm::vec3(0.0f, 0.0f, half_size), _colour, thickness);
	//top to bottom (Y axis) green
	_colour = (axis_colour) ? glm::vec3(0.0f, 1.0f, 0.0f) : colour;
	DrawLine(center + glm::vec3(0.0f, half_size, 0.0f), center - glm::vec3(0.0, half_size, 0.0f), _colour, thickness);
	//DrawRay(center + (glm::vec3(0.0f, half_size, 0.0f)), glm::vec3(0.0, -1.0f, 0.0f), size, _colour, thickness);
		
	//right to right (X axis) red
	_colour = (axis_colour) ? glm::vec3(1.0f, 0.0f, 0.0f) : colour;
	DrawLine(center + glm::vec3(half_size, 0.0f, 0.0f), center - glm::vec3(half_size, 0.0f, 0.0f), _colour, thickness);

}

glm::vec3 Perpen(const glm::vec3& v)
{
	glm::vec3 u(1.0f, 0.0f, 0.0f); //Arbitrary not parellel to v
	if (glm::length(glm::cross(v, u)) == 0)
		u = glm::vec3(0.0f, 1.0f, 0.0f);//if parellel choose another 
	return glm::cross(v, u);
}

void DebugGizmos::DrawWireDisc(glm::vec3 center, glm::vec3 right, glm::vec3 up, float radius, int step, glm::vec3 colour, float thickness)
{
	//Quick hack
	glm::vec3 prev = center + (right * radius);
	for (float theta = 0.0f; theta < 2.0f * MathsHelper::PI; theta += (MathsHelper::PI/step))
	{
		glm::vec3 p = center + radius * (right * glm::cos(theta) + up * glm::sin(theta));
		DrawLine(prev, p, colour, thickness);
		prev = p;
	}
	//last with first
	DrawLine(prev, center + (right * radius), colour, thickness);

}

void DebugGizmos::DrawWireThreeDisc(glm::vec3 center, float radius, int steps, glm::vec3 colour, float thickness)
{
	DrawWireDisc(center, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), radius, steps, colour, thickness);
	DrawWireDisc(center, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), radius, steps, colour, thickness);
	DrawWireDisc(center, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), radius, steps, colour, thickness);
}

void DebugGizmos::DrawWireCone(glm::vec3 center, glm::vec3 up, glm::vec3 right, float radius, float height, glm::vec3 colour, float thickness)
{
	//debug center
	DrawCross(center);

	//Quick hack
	//glm::vec3 forward = Perpen(up);
	//glm::vec3 right = glm::cross(up, forward);
	glm::vec3 peak = center + (up * height);
	glm::vec3 forward = glm::cross(up, right);
	glm::vec3 prev = center + (right * radius);
	//center to start
	DrawLine(center, prev, colour, thickness);
	DrawLine(prev, peak, colour, thickness);

	for (float theta = 0.0f; theta < 2.0f * MathsHelper::PI; theta += (MathsHelper::PI / 5))
	{
		glm::vec3 p = center + radius * (right * glm::cos(theta) + forward * glm::sin(theta));
		DrawLine(prev, p, colour, thickness);
		DrawLine(p, peak, colour, thickness);
		prev = p;
	}
	//last with first
	DrawLine(prev, center + (right * radius), colour, thickness);
}

void DebugGizmos::DrawOrthoCameraFrustrm(glm::vec3 pos, glm::vec3 forward, float cam_near, float cam_far, float size, glm::vec3 colour, float thickness)
{
	if (!active)
		Generate();

	glm::vec3 rightV = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 0.0f, 1.0f)));
	glm::vec3 upV = glm::normalize(glm::cross(rightV, forward));

	glm::vec3 boundsTL = (upV * size) + (rightV * -size);
	glm::vec3 boundsTR = (upV * size) + (rightV * size);
	glm::vec3 boundsBL = (upV * -size) + (rightV * -size);
	glm::vec3 boundsBR = (upV * -size) + (rightV * size);

	glm::vec3 nearPlane = pos + (glm::normalize(-forward) * cam_near);

	glm::vec3 nearTL = nearPlane + boundsTL;
	glm::vec3 nearTR = nearPlane + boundsTR;
	glm::vec3 nearBL = nearPlane + boundsBL;
	glm::vec3 nearBR = nearPlane + boundsBR;

	glm::vec3 farPlane = pos + (glm::normalize(-forward) * cam_far);


	glm::vec3 farTR = farPlane + boundsTL;
	glm::vec3 farTL = farPlane + boundsTR;
	glm::vec3 farBR = farPlane + boundsBL;
	glm::vec3 farBL = farPlane + boundsBR;


	//Debuging Camera position
	DrawSphere(pos, 0.2f);

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

void DebugGizmos::DrawPerspectiveCameraFrustum(glm::vec3 pos, glm::vec3 forward, float fov, float aspect, float cam_near, float cam_far, glm::vec3 colour, float thickness)
{
	if (!active)
		Generate();

	glm::vec3 rightV = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 0.0f, 1.0f)));
	glm::vec3 upV = glm::normalize(glm::cross(rightV, forward));


	//near
	float size_ratio; 
	size_ratio = glm::tan(glm::radians(fov * 0.5f)) * cam_near;
	//size_ratio *= aspect;


	glm::vec3 boundsTL = (upV * size_ratio) + (rightV * -size_ratio);
	glm::vec3 boundsTR = (upV * size_ratio) + (rightV * size_ratio);
	glm::vec3 boundsBL = (upV * -size_ratio) + (rightV * -size_ratio);
	glm::vec3 boundsBR = (upV * -size_ratio) + (rightV * size_ratio);

	glm::vec3 nearPlane = pos + (glm::normalize(-forward) * cam_near);

	glm::vec3 nearTL = nearPlane + boundsTL;
	glm::vec3 nearTR = nearPlane + boundsTR;
	glm::vec3 nearBL = nearPlane + boundsBL;
	glm::vec3 nearBR = nearPlane + boundsBR;


	//far
	size_ratio = glm::tan(glm::radians(fov * 0.5f)) * cam_far;
	size_ratio *= aspect;

	boundsTL = (upV * size_ratio) + (rightV * -size_ratio);
	boundsTR = (upV * size_ratio) + (rightV * size_ratio);
	boundsBL = (upV * -size_ratio) + (rightV * -size_ratio);
	boundsBR = (upV * -size_ratio) + (rightV * size_ratio);

	glm::vec3 farPlane = pos + (glm::normalize(-forward) * cam_far);

	glm::vec3 farTR = farPlane + boundsTL;
	glm::vec3 farTL = farPlane + boundsTR;
	glm::vec3 farBR = farPlane + boundsBL;
	glm::vec3 farBL = farPlane + boundsBR;

	//DrawSphere(pos, 0.2f);
	DrawCross(pos);

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

void DebugGizmos::DrawPerspectiveCameraFrustum(glm::vec3 pos, glm::vec3 forward, glm::vec3 up, float fov, float aspect, float cam_near, float cam_far, glm::vec3 colour, float thickness)
{
	if (!active)
		Generate();

	glm::vec3 rightV = glm::normalize(glm::cross(forward, up));
	glm::vec3 upV = glm::normalize(glm::cross(rightV, forward));


	//near
	float size_ratio;
	size_ratio = glm::tan(glm::radians(fov * 0.5f)) * cam_near;
	//size_ratio *= aspect;


	glm::vec3 boundsTL = (upV * size_ratio) + (rightV * -size_ratio);
	glm::vec3 boundsTR = (upV * size_ratio) + (rightV * size_ratio);
	glm::vec3 boundsBL = (upV * -size_ratio) + (rightV * -size_ratio);
	glm::vec3 boundsBR = (upV * -size_ratio) + (rightV * size_ratio);

	glm::vec3 nearPlane = pos + (glm::normalize(-forward) * cam_near);

	glm::vec3 nearTL = nearPlane + boundsTL;
	glm::vec3 nearTR = nearPlane + boundsTR;
	glm::vec3 nearBL = nearPlane + boundsBL;
	glm::vec3 nearBR = nearPlane + boundsBR;


	//far
	size_ratio = glm::tan(glm::radians(fov * 0.5f)) * cam_far;
	size_ratio *= aspect;

	boundsTL = (upV * size_ratio) + (rightV * -size_ratio);
	boundsTR = (upV * size_ratio) + (rightV * size_ratio);
	boundsBL = (upV * -size_ratio) + (rightV * -size_ratio);
	boundsBR = (upV * -size_ratio) + (rightV * size_ratio);

	glm::vec3 farPlane = pos + (glm::normalize(-forward) * cam_far);

	glm::vec3 farTR = farPlane + boundsTL;
	glm::vec3 farTL = farPlane + boundsTR;
	glm::vec3 farBR = farPlane + boundsBL;
	glm::vec3 farBL = farPlane + boundsBR;

	DrawSphere(pos, 0.2f);
	//DrawCross(pos);

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
