#include "Camera.h"
#include "Util/MathsHelpers.h"

#include <iostream>


Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat moveSpeed, GLfloat rotSpeed)
	:  m_Position(startPosition), m_WorldUp(startUp), m_Yaw(startYaw),
	   m_Pitch(startPitch), m_MoveSpeed(moveSpeed), m_RotSpeed(rotSpeed)
{

	m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Up = glm::vec3(0.0f);
	m_Right = glm::vec3(0.0f);

	Update();
}


Camera::~Camera()
{
}


glm::mat4 Camera::CalViewMat()
{
	return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::Translate(glm::vec3 direction, float dt)
{
	//m_Position += (m_Front * displacement * m_MoveSpeed * dt);

	glm::vec3 dir(direction.x, direction.y, direction.z);
	m_Position += (dir * m_MoveSpeed * dt);
}

void Camera::Rotate(float dt_yaw, float dt_pitch)
{
	float sensitivity = 0.1f;

	m_Yaw += dt_yaw * m_RotSpeed * sensitivity;
	m_Pitch += dt_pitch * m_RotSpeed * sensitivity;


	if (m_Yaw > 360.0f)
		m_Yaw = m_Yaw - 360;
	if (m_Yaw < 0)
		m_Yaw = 360 - m_Yaw;


	if (m_Pitch > 89.0f)
		m_Pitch = 89.0f;
	if (m_Pitch < -89.0f)
		m_Pitch = -89.0f;

	Update();
}

void Camera::Rotate(glm::vec2 mouse_pos, float height, float width)
{
	glm::vec2 mouse_movement = mouse_pos - (glm::vec2(width, height) / 2.0f);

	m_Pitch = m_Front.x - (int)mouse_movement.y * 0.1f;
	m_Yaw = m_Front.y + (int)mouse_movement.x * 0.1f;

	//Just an hack should not affect the system 

	//if (m_Yaw > 360.0f)
	//	m_Yaw = m_Yaw - 360;
	//if (m_Yaw < 0)
	//	m_Yaw = 360 - m_Yaw;


	if (m_Pitch > 89.0f)
		m_Pitch = 89.0f;
	if (m_Pitch < -89.0f)
		m_Pitch = -89.0f;

	Update();
}



void Camera::SetPosition(glm::vec3 pos)
{
	m_Position = pos;
}

void Camera::Update()
{
	//m_Front.x = cos(MathsHelper::Deg2Rad(m_Yaw)) * cos(MathsHelper::Deg2Rad(m_Pitch));
	//m_Front.y = sin(MathsHelper::Deg2Rad(m_Pitch));
	//m_Front.z = sin(MathsHelper::Deg2Rad(m_Yaw)) * cos(MathsHelper::Deg2Rad(m_Pitch));
	//m_Front.Normalize();

	//m_Right = (m_Front.Cross(m_WorldUp)).Normalize();
	//m_Up = (m_Right.Cross(m_Front)).Normalize();

	m_Front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front.y = sin(glm::radians(m_Pitch));
	m_Front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(m_Front);

	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
