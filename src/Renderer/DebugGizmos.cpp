#include "DebugGizmos.h"

#include "RendererErrorAssertion.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Util/MathsHelpers.h"

#include "Meshes/PrimitiveMeshFactory.h"

bool DebugGizmos::active = false;
bool DebugGizmos::use = true;
Shader DebugGizmos::m_Shader;
Mesh DebugGizmos::sphere;
CRRT::SceneRenderer DebugGizmos::m_Renderer;

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
	if (!use)
		return;

	if (!active)
		Generate();


	m_Shader.Bind();
	glm::mat4 model = glm::mat4(1.0f);
	m_Shader.SetUniformMat4f("u_Model", model);
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(colour, 1.0f));
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
	if (!use)
		return;

	DrawLine(v1, v1 + (dir * dist), colour, thickness);
}

void DebugGizmos::DrawWireSphere(glm::vec3 p, float radius, glm::vec3 colour, float thickness)
{
	if (!use)
		return;

	if (!active)
		Generate();

	m_Shader.Bind();
	glm::mat4 model = glm::mat4(1.0f);			
	model = glm::translate(model, p);
	model = glm::scale(model, glm::vec3(radius));
	m_Shader.SetUniformMat4f("u_Model", model);
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(colour, 1.0f));
	//sphere.RenderDebugOutLine(thickness);
	m_Renderer.DrawMeshOutline(sphere);
	m_Shader.UnBind();
}

void DebugGizmos::DrawSphere(glm::vec3 p, float radius, glm::vec3 colour)
{
	if (!use)
		return;

	if (!active)
		Generate();

	m_Shader.Bind();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, p);
	model = glm::scale(model, glm::vec3(radius));
	m_Shader.SetUniformMat4f("u_Model", model);
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(colour, 1.0f));
	//sphere.Render();
	m_Renderer.DrawMesh(sphere);
	m_Shader.UnBind();
}

void DebugGizmos::DrawSquare(glm::vec3 center, glm::vec3 forward, float left, float right, float bottom, float top, glm::vec3 colour, float thickness)
{
	if (!use)
		return;

	if (!active)
		Generate();

	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_Model", glm::mat4(1.0f));
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(colour, 1.0f));

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
	if (!use)
		return;

	if (!active)
		Generate();

	
	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_Model", glm::mat4(1.0f));
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(colour, 1.0f));

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
	if (!use)
		return;

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
	if (!use)
		return;

	//Quick hack
	glm::vec3 prev = center + (right * radius);
	for (float theta = 0.0f; theta < 2.0f * (float)MathsHelper::PI; theta += ((float)MathsHelper::PI/(float)step))
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
	if (!use)
		return;

	DrawWireDisc(center, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), radius, steps, colour, thickness);
	DrawWireDisc(center, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), radius, steps, colour, thickness);
	DrawWireDisc(center, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), radius, steps, colour, thickness);
}

void DebugGizmos::DrawWireCone(glm::vec3 center, glm::vec3 up, glm::vec3 right, float radius, float height, glm::vec3 colour, float thickness)
{
	if (!use)
		return;

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

	for (float theta = 0.0f; theta < 2.0f * (float)MathsHelper::PI; theta += ((float)MathsHelper::PI / 5.0f))
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
	if (!use)
		return;

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
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(colour, 1.0f));

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
	if (!use)
		return;

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
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(colour, 1.0f));

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
	if (!use)
		return;

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
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(colour, 1.0f));

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

void DebugGizmos::DrawPlane(const Plane& f, glm::vec2 size, glm::vec3 col, float thickness)
{
	if (!use)
		return;

	if (!active)
		Generate();

	float d = f.GetConstant();
	glm::vec3 N = f.GetNormal();
	N = glm::normalize(N);
	glm::vec3 center = -d * N;

	glm::vec3 arbitrary_vec;
	if (std::fabs(N.x) > std::fabs(N.z))
		arbitrary_vec = glm::vec3(-N.y, N.x, 0.0f);
	else
		arbitrary_vec = glm::vec3(0.0f, -N.z, N.y);

	glm::vec3 u = glm::cross(N, arbitrary_vec);
	glm::vec3 v = glm::cross(N, u);

	u *= size.x;
	v *= size.y;

	glm::vec3 p1 = center + ((u + v));
	glm::vec3 p2 = center + ((u - v));
	glm::vec3 p3 = center - ((u + v));
	glm::vec3 p4 = center - ((u - v));

	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_Model", glm::mat4(1.0f));
	//m_Shader.SetUniformVec3("u_Colour", col);
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(col, 0.5f));

	//for transparency 
	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);
	glBegin(GL_TRIANGLES);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glVertex3f(p3.x, p3.y, p3.z);

		glVertex3f(p3.x, p3.y, p3.z);
		glVertex3f(p4.x, p4.y, p4.z);
		glVertex3f(p1.x, p1.y, p1.z);
	glEnd();
	m_Shader.UnBind();
	glEnable(GL_CULL_FACE);

	glDisable(GL_BLEND);
	m_Shader.UnBind();
}



void DebugGizmos::DrawWirePlane(const Plane& f, glm::vec2 size, glm::vec3 col, float thickness)
{
	if (!use)
		return;

	if (!active)
		Generate();

	float d = f.GetConstant();
	glm::vec3 N = f.GetNormal();
	N = glm::normalize(N);
	glm::vec3 center = -d * N;

	glm::vec3 arbitrary_vec;
	if (std::fabs(N.x) > std::fabs(N.z))
		arbitrary_vec = glm::vec3(-N.y, N.x, 0.0f);
	else
		arbitrary_vec = glm::vec3(0.0f, -N.z, N.y);

	glm::vec3 u = glm::cross(N, arbitrary_vec);
	glm::vec3 v = glm::cross(N, u);


	u *= size.x;
	v *= size.y;

	glm::vec3 p1 = center + ((u + v));
	glm::vec3 p2 = center + ((u - v));
	glm::vec3 p3 = center - ((u + v));
	glm::vec3 p4 = center - ((u - v));

	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_Model", glm::mat4(1.0f));
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(col, 1.0f));
	glBegin(GL_LINE_LOOP);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glVertex3f(p3.x, p3.y, p3.z);
		glVertex3f(p4.x, p4.y, p4.z);
		//extra for visuals
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p3.x, p3.y, p3.z);
	glEnd();
	m_Shader.UnBind();
}

void DebugGizmos::DrawFrustum(const Frustum& f, glm::vec3 col, float thickness)
{
	if (!use)
		return;

	if (!active)
		Generate();


	Plane lt_plane = f.GetPlane(Planes_side::Left);
	Plane rt_plane = f.GetPlane(Planes_side::Right);
	Plane nr_plane = f.GetPlane(Planes_side::Near);
	Plane tp_plane = f.GetPlane(Planes_side::Top);
	Plane bm_plane = f.GetPlane(Planes_side::Bottom);
	Plane fr_plane = f.GetPlane(Planes_side::Far);

	//near pts top_lt >> bottom_lt >> bottom_rt >> top_rt
	glm::vec3 p1 = glm::vec3(0.0f); 
	glm::vec3 p2 = glm::vec3(0.0f);
	glm::vec3 p3 = glm::vec3(0.0f);
	glm::vec3 p4 = glm::vec3(0.0f);
	Plane::IntersectThreePlanes(tp_plane, nr_plane, lt_plane, p1);
	Plane::IntersectThreePlanes(bm_plane, nr_plane, lt_plane, p2);
	Plane::IntersectThreePlanes(bm_plane, nr_plane, rt_plane, p3);
	Plane::IntersectThreePlanes(tp_plane, nr_plane, rt_plane, p4);

	//debug sphere 
	DrawSphere(p1, 0.5f, col);
	DrawSphere(p2, 0.5f, col);
	DrawSphere(p3, 0.5f, col);
	DrawSphere(p4, 0.5f, col);


	//far pts top_lt >> bottom_lt >> bottom_rt >> top_rt
	glm::vec3 p5 = glm::vec3(0.0f);
	glm::vec3 p6 = glm::vec3(0.0f);
	glm::vec3 p7 = glm::vec3(0.0f);
	glm::vec3 p8 = glm::vec3(0.0f);
	Plane::IntersectThreePlanes(tp_plane, fr_plane, lt_plane, p5);
	Plane::IntersectThreePlanes(bm_plane, fr_plane, lt_plane, p6);
	Plane::IntersectThreePlanes(bm_plane, fr_plane, rt_plane, p7);
	Plane::IntersectThreePlanes(tp_plane, fr_plane, rt_plane, p8);


	//debug sphere 
	DrawSphere(p5, 0.5f, col);
	DrawSphere(p6, 0.5f, col);
	DrawSphere(p7, 0.5f, col);
	DrawSphere(p8, 0.5f, col);


	DrawLine(p1, p5, col);
	DrawLine(p2, p6, col);
	DrawLine(p3, p7, col);
	DrawLine(p4, p8, col);

	//return;
	//Draw solid colours
	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_Model", glm::mat4(1.0f));
	m_Shader.SetUniformVec4("u_Colour", glm::vec4(col, 0.2f));
	glDisable(GL_CULL_FACE);
	glBegin(GL_LINE_LOOP);
		glVertex3fv(&p1[0]);
		glVertex3fv(&p2[0]);
		glVertex3fv(&p3[0]);
		glVertex3fv(&p4[0]);
	glEnd();
	glBegin(GL_TRIANGLES);
	//left plane 
		glVertex3fv(&p7[0]);
		glVertex3fv(&p3[0]);
		glVertex3fv(&p4[0]);

		glVertex3fv(&p8[0]);
		glVertex3fv(&p7[0]);
		glVertex3fv(&p4[0]);

	//top plane
		glVertex3fv(&p8[0]);//glVertex3fv(&p1[0]);
		glVertex3fv(&p4[0]);//glVertex3fv(&p4[0]);
		glVertex3fv(&p1[0]);//glVertex3fv(&p8[0]);

		glVertex3fv(&p5[0]);//glVertex3fv(&p1[0]);
		glVertex3fv(&p8[0]);//glVertex3fv(&p8[0]);
		glVertex3fv(&p1[0]);//glVertex3fv(&p5[0]);

	//right plane 
		glVertex3fv(&p6[0]);
		glVertex3fv(&p5[0]);
		glVertex3fv(&p1[0]);

		glVertex3fv(&p6[0]);
		glVertex3fv(&p1[0]);
		glVertex3fv(&p2[0]);

	//bottom plane 
		glVertex3fv(&p6[0]);
		glVertex3fv(&p7[0]);
		glVertex3fv(&p2[0]);

		glVertex3fv(&p2[0]);
		glVertex3fv(&p7[0]);
		glVertex3fv(&p3[0]);
	glEnd();

	//near plane 
	glEnable(GL_CULL_FACE);
	glBegin(GL_TRIANGLES);
		glVertex3fv(&p1[0]);
		glVertex3fv(&p2[0]);
		glVertex3fv(&p4[0]);

		glVertex3fv(&p4[0]);
		glVertex3fv(&p2[0]);
		glVertex3fv(&p3[0]);

	glEnd();
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
		CRRT_ASSETS_PATH"/Shaders/Utilities/Debugger/PrimitiveVertex.glsl", //vertex shader
		CRRT_ASSETS_PATH"/Shaders/Utilities/Debugger/PrimitiveFrag.glsl", //fragment shader
	};
	m_Shader.Create("debug_shader", debug_shader_file_path);

	sphere = CRRT::PrimitiveMeshFactory::Instance().CreateASphere(12, 6);

	active = true;

}
