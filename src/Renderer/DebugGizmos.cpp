#include "DebugGizmos.h"

#include "RendererErrorAssertion.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Util/MathsHelpers.h"

#include "Meshes/PrimitiveMeshFactory.h"

#include "Geometry/AABB.h"
#include "Geometry/Plane.h"
#include "Geometry/Frustum.h"


bool DebugGizmos::active = false;
bool DebugGizmos::use = true;
Mesh DebugGizmos::sphere;
CRRT::SceneRenderer DebugGizmos::m_Renderer;

Shader DebugGizmos::m_DebugMeshShader;
Shader DebugGizmos::m_BatchLineShader;
Shader DebugGizmos::m_InstanceShader;

DebugGizmos::LineSegmentVertex DebugGizmos::m_LineSegVertex;
DebugGizmos::VertexObjData DebugGizmos::m_SphereVertexData;

std::vector<DebugGizmos::Line> DebugGizmos::m_BatchLines;
std::vector<DebugGizmos::Triangle> DebugGizmos::m_BatchTriangles;
std::vector<DebugGizmos::Triangle> DebugGizmos::m_BatchNoCullFaceTriangles;
std::vector<DebugGizmos::VertexInstanceData> DebugGizmos::m_WireSphereInstance;
std::vector<DebugGizmos::VertexInstanceData> DebugGizmos::m_SolidSphereInstance;




void DebugGizmos::Startup()
{
	Generate();
}

DebugGizmos::~DebugGizmos()
{
	Cleanup();
}



void DebugGizmos::DrawTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec4 colour, bool cull_face)
{

	if (!use)
		return;

	if (!active)
		Generate();

	BatchTriangle({ v0.x, v0.y, v0.z, colour.r, colour.g, colour.b,	colour.a,	// <--- v0
					v1.x, v1.y, v1.z, colour.r, colour.g, colour.b,	colour.a,	// <--- v1
					v2.x, v2.y, v2.z, colour.r, colour.g, colour.b,	colour.a,	// <--- v2
					}, cull_face);

}

void DebugGizmos::DrawTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec4 col0, glm::vec4 col1, glm::vec4 col2, bool cull_face)
{
	if (!use)
		return;

	if (!active)
		Generate();

	BatchTriangle({ v0.x, v0.y, v0.z, col0.r, col0.g, col0.b, col0.a,	// <--- v0
					v1.x, v1.y, v1.z, col1.r, col1.g, col1.b, col1.a,	// <--- v1
					v2.x, v2.y, v2.z, col2.r, col2.g, col2.b, col2.a,	// <--- v2
		}, cull_face);
}

void DebugGizmos::DrawWireTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 colour, float thickness)
{
	DrawLine(v1, v2, colour);
	DrawLine(v1, v3, colour);
	DrawLine(v2, v3, colour);
}

//Make sure Camera data uniform buffer is generated & update d
void DebugGizmos::DrawLine(glm::vec3 v1, glm::vec3 v2, glm::vec3 colour, float thickness)
{
	if (!use)
		return;

	if (!active)
		Generate();

	BatchLineSegment({ v1.x, v1.y, v1.z, colour.x, colour.y, colour.z, 1.0f,	// <-- line v0
					   v2.x, v2.y, v2.z, colour.x, colour.y, colour.z, 1.0f	// <-- line v1
					 });
}

void DebugGizmos::DrawLine(glm::vec3 v0, glm::vec3 v1, glm::vec4 col0, glm::vec4 col1, float thickness)
{
	if (!use)
		return;

	if (!active)
		Generate();

	BatchLineSegment({ v0.x, v0.y, v0.z, col0.r, col0.g, col0.b, col0.a,	// <-- line v0
				       v1.x, v1.y, v1.z, col1.r, col1.g, col1.b, col1.a	// <-- line v1
					});
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


	VertexInstanceData data =
	{
		glm::vec4(colour.r, colour.g, colour.b, 1.0f),
		glm::translate(glm::mat4(1.0f), p)* glm::scale(glm::mat4(1.0f), glm::vec3(radius)),
	};
	CacheWireSphereInstance(data);
}

void DebugGizmos::DrawSphere(glm::vec3 p, float radius, glm::vec3 colour)
{
	if (!use)
		return;

	if (!active)
		Generate();

	VertexInstanceData data =
	{
		glm::vec4(colour.r, colour.g, colour.b, 1.0f),
		glm::translate(glm::mat4(1.0f), p) * glm::scale(glm::mat4(1.0f), glm::vec3(radius)),
	};
	CacheSolidSphereInstance(data);
}

void DebugGizmos::DrawSquare(glm::vec3 center, glm::vec3 forward, float left, float right, float bottom, float top, glm::vec3 colour, float thickness)
{
	if (!use)
		return;

	if (!active)
		Generate();

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

	DrawLine(v1, v2, colour, thickness);
	DrawLine(v2, v3, colour, thickness);
	DrawLine(v3, v4, colour, thickness);
	DrawLine(v4, v1, colour, thickness);
}


void DebugGizmos::DrawBox(AABB aabb, glm::vec3 colour, float thickness)
{
	if (!use)
		return;

	if (!active)
		Generate();

	//v1/////////v2//
	//############///
	//#//////////#///
	//#//////////#///
	//############///
	//v3/////////v4//
	//front box/square  v1>>v2>>v4>>v3
	glm::vec3 a = aabb.m_Min;
	glm::vec3 b = aabb.m_Max;
	DrawLine(a, glm::vec3(b.x, a.y, a.z), colour);
	DrawLine(glm::vec3(b.x, a.y, a.z), glm::vec3(b.x, b.y, a.z), colour);
	DrawLine(glm::vec3(a.x, b.y, a.z), glm::vec3(b.x, b.y, a.z), colour);
	DrawLine(glm::vec3(a.x, b.y, a.z), a, colour);

	//v3>>v7
	//v5/////////v6//
	//############///
	//#//////////#///
	//#//////////#///
	//############///
	//v7/////////v8//
	//back box/square v7>>v8>>v6>>v5
	DrawLine(glm::vec3(a.x, b.y, b.z), b, colour);
	DrawLine(b, glm::vec3(b.x, a.y, b.z), colour);
	DrawLine(glm::vec3(b.x, a.y, b.z), glm::vec3(a.x, a.y, b.z), colour);
	DrawLine(glm::vec3(a.x, a.y, b.z), glm::vec3(a.x, b.y, b.z), colour);


	////front left v1>>v5
	DrawLine(a, glm::vec3(a.x, a.y, b.z), colour);

	////back left v3>>v7
	DrawLine(glm::vec3(a.x, b.y, a.z), glm::vec3(a.x, b.y, b.z), colour);

	////front top >> back top v2>>v6
	DrawLine(glm::vec3(b.x, a.y, a.z), glm::vec3(b.x, a.y, b.z), colour);

	////front bottom >> back bottom v4>>v8
	DrawLine(glm::vec3(b.x, b.y, a.z), glm::vec3(b.x, b.y, b.z), colour);

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

	//near plane bounds vertex draw
	DrawLine(nearTL, nearTR, colour, thickness);
	DrawLine(nearTR, nearBR, colour, thickness);
	DrawLine(nearBR, nearBL, colour, thickness);
	DrawLine(nearBL, nearTL, colour, thickness);
	//far plane bounds vertex draw
	DrawLine(farBR, farBL, colour, thickness);
	DrawLine(farBL, farTL, colour, thickness);
	DrawLine(farTL, farTR, colour, thickness);
	DrawLine(farTR, farBR, colour, thickness);

	DrawLine(nearTL, nearBL, colour, thickness);

	DrawLine(farBR, farTR, colour, thickness);

	DrawLine(nearBR, farBL, colour, thickness);

	DrawLine(nearTR, farTL, colour, thickness);

	DrawLine(nearPlane, glm::vec3(nearPlane - (forward * 4.0f)), colour, thickness);
}

void DebugGizmos::DrawPerspectiveCameraFrustum(glm::vec3 pos, glm::vec3 forward, float fov, float aspect, float cam_near, float cam_far, glm::vec3 colour, float thickness)
{
	//assuming vec3(0,1,0) is perpendicular to the forward
	glm::vec3 arbitrary_vec;
	if (std::fabs(forward.x) > std::fabs(forward.z))
		arbitrary_vec = glm::vec3(-forward.y, forward.x, 0.0f);
	else
		arbitrary_vec = glm::vec3(0.0f, -forward.z, forward.y);
	DrawPerspectiveCameraFrustum(pos, forward, arbitrary_vec, fov, aspect, cam_near, cam_far, colour, thickness);
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

	//DrawSphere(pos, 0.2f);
	DrawCross(pos);
	
	//near plane bounds vertex draw
	DrawLine(nearTL, nearTR, colour);
	DrawLine(nearTR, nearBR, colour);
	DrawLine(nearBR, nearBL, colour);
	DrawLine(nearBL, farBR, colour);
	//far plane bounds vertex draw
	DrawLine(farBR, farBL, colour);
	DrawLine(farBL, farTL, colour);
	DrawLine(farTL, farTR, colour);
	DrawLine(farTR, nearTL, colour);

	DrawLine(nearTL, nearBL, colour);

	DrawLine(farBR, farTR, colour);

	DrawLine(nearBR, farBL, colour);

	DrawLine(nearTR, farTL, colour);

	DrawLine(nearPlane, glm::vec3(nearPlane - (glm::normalize(forward) * 4.0f)), colour);
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

	DrawTriangle(p1, p2, p3, glm::vec4(col, 1.0f), false);
	DrawTriangle(p3, p4, p1, glm::vec4(col, 1.0f), false);
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


	DrawLine(p1, p2, col);
	DrawLine(p2, p3, col);
	DrawLine(p3, p4, col);
	DrawLine(p4, p1, col);
	//extra for visuals
	DrawLine(p1, p3, col);
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
	DrawLine(p1, p2, col);
	DrawLine(p2, p3, col);
	DrawLine(p3, p4, col);
	DrawLine(p4, p1, col);


	//left plane 
	DrawTriangle(p7, p3, p4, glm::vec4(col, 0.5f), false);
	DrawTriangle(p8, p7, p4, glm::vec4(col, 0.5f), false);
	//top plane
	DrawTriangle(p8, p4, p1, glm::vec4(col, 0.5f), false);
	DrawTriangle(p5, p8, p1, glm::vec4(col, 0.5f), false);
	//right plane
	DrawTriangle(p6, p5, p1, glm::vec4(col, 0.5f), false);
	DrawTriangle(p6, p1, p2, glm::vec4(col, 0.5f), false);
	//bottom plane
	DrawTriangle(p6, p7, p2, glm::vec4(col, 0.5f), false);
	DrawTriangle(p2, p7, p3, glm::vec4(col, 0.5f), false);

	//glVertex3fv(&p1[0]);
	//glVertex3fv(&p2[0]);
	//glVertex3fv(&p4[0]);

	//glVertex3fv(&p4[0]);
	//glVertex3fv(&p2[0]);
	//glVertex3fv(&p3[0]);
}

void DebugGizmos::Cleanup()
{
	printf("Debugger clean up!!!!!!\n");
	//clean up
	m_DebugMeshShader.Clear();
	sphere.Clear();

	m_BatchLineShader.Clear();
	m_InstanceShader.Clear();

	glDeleteBuffers(1, &m_LineSegVertex.VAO);
	glDeleteBuffers(1, &m_LineSegVertex.VBO);

	glDeleteBuffers(1, &m_SphereVertexData.vao);
	glDeleteBuffers(1, &m_SphereVertexData.vbo);
	glDeleteBuffers(1, &m_SphereVertexData.ibo);
	glDeleteBuffers(1, &m_SphereVertexData.vbo2);

	m_BatchLines.clear();
	m_BatchTriangles.clear();
	m_BatchNoCullFaceTriangles.clear();

	m_WireSphereInstance.clear();
	m_SolidSphereInstance.clear();
}

void DebugGizmos::DrawAllBatches()
{
	DrawInstances();
	if (m_BatchLines.size() <= 0 && m_BatchTriangles.size() <= 0 && m_BatchNoCullFaceTriangles.size() <= 0)
		return;

	m_BatchLineShader.Bind();
	GLCall(glBindVertexArray(m_LineSegVertex.VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_LineSegVertex.VBO));


	//since both line and triangle starts at the idx 0 the max require would be fine
	unsigned int req_buffer_data_count = glm::max(m_BatchLines.size(), glm::max(m_BatchTriangles.size(), m_BatchNoCullFaceTriangles.size()));
	if (m_LineSegVertex.curr_vertex_count < req_buffer_data_count)
	{
		m_LineSegVertex.curr_vertex_count = req_buffer_data_count;
		//reallocate buffer to accomodate buffer, based on if triangle size if larger than line or vice versa
		GLsizeiptr buffer_data_size = glm::max(sizeof(Line) * m_BatchLines.size(), glm::max(sizeof(Triangle) * m_BatchTriangles.size(), 
														sizeof(Triangle) * m_BatchNoCullFaceTriangles.size()));
		GLCall(glBufferData(GL_ARRAY_BUFFER, buffer_data_size, nullptr, GL_DYNAMIC_DRAW));
	}


	//Draw batched Line segments
	if (m_BatchLines.size() > 0)
	{
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * m_BatchLines.size(), m_BatchLines.data(), GL_STATIC_DRAW);
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Line) * m_BatchLines.size(), m_BatchLines.data()));

		GLCall(glLineWidth(3.0f));
		GLCall(glDrawArrays(GL_LINES, 0, m_BatchLines.size() * 2));		// <-- x2 as Line as 2 points) 
		m_BatchLines.clear();
	}


	//Draw batched triangles with face culling
	if (m_BatchTriangles.size() > 0)
	{
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * m_BatchTriangles.size(), m_BatchTriangles.data(), GL_STATIC_DRAW);
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Triangle) * m_BatchTriangles.size(), m_BatchTriangles.data()));
		GLCall(glDrawArrays(GL_TRIANGLES, 0, m_BatchTriangles.size() * 3));		// <-- x3 as Triangle as 3 points) 
		m_BatchTriangles.clear();
	}

	//Draw batched triangles without face culling
	if (m_BatchNoCullFaceTriangles.size() > 0)
	{
		////for transparency 
		GLCall(glEnable(GL_BLEND));
		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Triangle) * m_BatchNoCullFaceTriangles.size(), m_BatchNoCullFaceTriangles.data()));
		GLCall(glDrawArrays(GL_TRIANGLES, 0, m_BatchNoCullFaceTriangles.size() * 3));		// <-- x3 as Triangle as 3 points) 
		m_BatchNoCullFaceTriangles.clear();
		GLCall(glEnable(GL_CULL_FACE));
		GLCall(glDisable(GL_BLEND));
	}

	GLCall(glBindVertexArray(0));
	m_BatchLineShader.UnBind();


}

void DebugGizmos::BatchLineSegment(const Line& line)
{
	m_BatchLines.push_back(line);
}

void DebugGizmos::BatchTriangle(const Triangle& tri, bool cull_face)
{
	if (cull_face)
		m_BatchTriangles.push_back(tri);
	else
		m_BatchNoCullFaceTriangles.push_back(tri);
}

void DebugGizmos::CacheWireSphereInstance(const VertexInstanceData& trans)
{
	m_WireSphereInstance.push_back(trans);
}

void DebugGizmos::CacheSolidSphereInstance(const VertexInstanceData& trans)
{
	m_SolidSphereInstance.push_back(trans);
}

void DebugGizmos::DrawInstances()
{
	if (m_WireSphereInstance.size() < 0 && m_SolidSphereInstance.size() < 0)
		return;

	m_InstanceShader.Bind();
	GLCall(glBindVertexArray(m_SphereVertexData.vao));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_SphereVertexData.vbo2));
	
	//since both wire and solid starts at the idx 0 the max require would be fine
	unsigned int req_buffer_size = glm::max(m_WireSphereInstance.size(), m_SolidSphereInstance.size());
	if (m_SphereVertexData.curr_inst_count < req_buffer_size)
	{
		m_SphereVertexData.curr_inst_count = req_buffer_size;
		//reallocate buffer to accomodate instances 
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexInstanceData) * m_SphereVertexData.curr_inst_count, nullptr, GL_DYNAMIC_DRAW));
	}

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_SphereVertexData.ibo));

	//Draw Wire Sphere instances
	if (m_WireSphereInstance.size() > 0)
	{
		//updata instance data
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexInstanceData) * m_WireSphereInstance.size(), m_WireSphereInstance.data()));

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_SphereVertexData.ibo);
		GLCall(glLineWidth(1.5f));
		GLCall(glDrawElementsInstanced(GL_LINE_LOOP, m_SphereVertexData.indices_count, GL_UNSIGNED_INT, (void*)0, m_WireSphereInstance.size()));

		m_WireSphereInstance.clear();
	}


	//Draw Solid Sphere instance
	if (m_SolidSphereInstance.size() > 0)
	{
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexInstanceData) * m_SolidSphereInstance.size(), m_SolidSphereInstance.data()));
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_SphereVertexData.ibo);
		GLCall(glDrawElementsInstanced(GL_TRIANGLES, m_SphereVertexData.indices_count, GL_UNSIGNED_INT, (void*)0, m_SolidSphereInstance.size()));

		m_SolidSphereInstance.clear();
	}


	m_InstanceShader.UnBind();
	GLCall(glBindVertexArray(0));
}

void DebugGizmos::Generate()
{
	ShaderFilePath debug_shader_file_path
	{
		"Assets/Shaders/Utilities/Debugger/PrimitiveVertex.glsl", //vertex shader
		"Assets/Shaders/Utilities/Debugger/PrimitiveFrag.glsl", //fragment shader
	};
	m_DebugMeshShader.Create("debug_shader", debug_shader_file_path);

	sphere = CRRT::PrimitiveMeshFactory::Instance().CreateASphere(12, 6);

	active = true;



	//New Line Segment
	glGenVertexArrays(1, &m_LineSegVertex.VAO);
	//vertex buffer
	glGenBuffers(1, &m_LineSegVertex.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_LineSegVertex.VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Line), nullptr, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * m_LineSegVertex.curr_vertex_count, nullptr, GL_DYNAMIC_DRAW);
	glBindVertexArray(m_LineSegVertex.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_LineSegVertex.VBO);

	GLsizei layout_stride = sizeof(Line) / 2;  // <-- size of a vertex(since there are two do divided by 2)
	//GLsizei layout_stride = sizeof(NewLine::p0);
	unsigned int layout_offset = 0;

	//start point
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, layout_stride, (void*)layout_offset);
	layout_offset += 3 * sizeof(float); //start point
	//layout_offset += sizeof(Line::start);

	//start colour
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, layout_stride, (void*)layout_offset);
	layout_offset += 4 * sizeof(float); // start colour

	////end point
	//glEnableVertexAttribArray(2); // <--- Need enable vertex index attribute
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, layout_stride, (void*)layout_offset);
	//layout_offset += 3 * sizeof(float); // end point

	////end colour
	//glEnableVertexAttribArray(3); // <--- Need enable vertex index attribute
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, layout_stride, (void*)layout_offset);
	//layout_offset += 3 * sizeof(float); // end colour


	ShaderFilePath debug_shader_file_path3
	{
		"Assets/Shaders/Utilities/Debugger/LineSegment/BatchLines.vert", //vertex shader
		"Assets/Shaders/Utilities/Debugger/Instances/Instance.frag",
	};
	
	m_BatchLineShader.Create("debug_shader_2", debug_shader_file_path3);

	ShaderFilePath debug_shader_file_path4
	{
		"Assets/Shaders/Utilities/Debugger/Instances/Instance.vert", //vertex shader
		"Assets/Shaders/Utilities/Debugger/Instances/Instance.frag",
	};

	m_InstanceShader.Create("debug_shader_4", debug_shader_file_path4);




	//Sphere Vertex Data

	//debug sphere vertex
	//float pos[3]
	//float col[3]

	//mat4 instance_transform <--- upload to GPU later

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	//vextex position
	float x, y, z, w;
	//vectex texture coord
	float u, v;

	float sector_angle;
	float span_angle;

	unsigned int sector_count = 12;
	unsigned int span_count = 6;

	//TO-DO: for now use float, double will be too redundant to use 
	float sector_step = (float)(2 * MathsHelper::PI / sector_count);  // 0 - 360(2pi)/count =>  angle btw each steps
	float span_step = (float)(MathsHelper::PI / span_count);		  // 0 - 180(pi)/count => angle btw each step

	//float radius = 1.0f;
	float radius = 0.5f;

	//compute & store vertices
	for (unsigned int i = 0; i <= span_count; i++)
	{
		// 180 degree(pi) to 0 degree //0 degree to 180 degree(pi)
		span_angle = (float)MathsHelper::PI - i * span_step;

		//parametric equation for sphere
		// x = center_x + r * sin(theta) * cos(phi)    
		// y = center_y + r * sin(thata) * sin(phi)
		// z = center_z + r * cos(theta)
		// where 
		//		theta = span_angle range 0 to pi(180 degrees)
		//		phi = sector_angle range 0 to 2pi(360 degrees)
		//RETERIVED: https://en.m.wikipedia.org/wiki/Sphere
		//			In their case z is up axis


		w = radius * glm::sin(span_angle);
		y = radius * glm::cos(span_angle);

		//add (sector_count + 1) vertices per stack
		//first and last vertices have same position, but different tex coords 
		for (unsigned int j = 0; j <= sector_count; ++j)
		{
			//start from 0 >> current step * step angle >> 360 
			sector_angle = j * sector_step;

			//vectex position (x, y, z)
			x = w * glm::cos(sector_angle);			//r * cos(u) * cos(v)
			z = w * glm::sin(sector_angle);			//r * cos(u) * sin(v)

			//vertex texture coord range between [0, 1]
			u = (float)j / sector_count;
			v = (float)i / span_count;


			//debug sphere vertex
			//float pos[3]
			
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}


	//compute & store indices
	unsigned int k1, k2;
	for (unsigned int i = 0; i < span_count; ++i)
	{
		k1 = i * (sector_count + 1);		//beginning of current stack
		k2 = k1 + sector_count + 1;			//beginning of next stack

		for (unsigned int j = 0; j < sector_count; ++j, ++k1, ++k2)
		{
			//2 triangles per sector excluding first and last stacks
			//k1 => k2 => k1+1
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			//k1+1 => k2 => k2+ 1
			if (i != (span_count - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}

		}
	}

	m_SphereVertexData.indices_count = indices.size();
	glGenVertexArrays(1, &m_SphereVertexData.vao);
	glGenBuffers(1, &m_SphereVertexData.vbo);

	glBindVertexArray(m_SphereVertexData.vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_SphereVertexData.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_SphereVertexData.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_SphereVertexData.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_SphereVertexData.vbo);


	unsigned int total_stride = 3 * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, total_stride, (void*)0);

	glGenBuffers(1, &m_SphereVertexData.vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, m_SphereVertexData.vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexInstanceData) * m_SphereVertexData.curr_inst_count, nullptr, GL_DYNAMIC_DRAW);


	//instance
	//float 3 --> colour
	//mat4 --> transformation 
	layout_stride = sizeof(glm::vec4) + sizeof(glm::mat4); // <--- how far to go to get the next idx
	layout_offset = 0;
	//layout attribute location 1, 2, 3, 4, 5
	//---- location
	//vec3	  1
	// ----------------------------------
	//vec4	  2
	//vec4	  3
	//vec4	  4
	//vec4	  5
	//------mat4
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, layout_stride, (void*)layout_offset);
	glVertexAttribDivisor(1, 1);
	layout_offset += sizeof(glm::vec4);
	for (unsigned i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(2+i);
		glVertexAttribPointer(2+i, 4, GL_FLOAT, GL_FALSE, layout_stride, (void*)layout_offset);
		layout_offset += sizeof(glm::vec4);
		glVertexAttribDivisor(2+i, 1);
	}

	glBindVertexArray(0);

}
