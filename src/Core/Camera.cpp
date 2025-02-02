#include "Camera.h"
#include "Util/MathsHelpers.h"

#include <iostream>


Camera::Camera(glm::vec3 startPosition, glm::vec3 worldUp, float startYaw, float startPitch, float moveSpeed, float rotSpeed)
	:  m_Position(startPosition), m_WorldUp(worldUp), m_Yaw(startYaw),
	   m_Pitch(startPitch), m_MoveSpeed(moveSpeed), m_RotSpeed(rotSpeed)
{
	Update();
}


Camera::Camera()
{
	Update();
}

Camera::~Camera()
{
}


glm::mat4 Camera::CalViewMat()
{
	Update();
	return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::CalculateProjMatrix(float aspect_ratio) const
{
	return glm::perspective(glm::radians(m_FOV), aspect_ratio, m_Near, m_Far);
}

void Camera::Translate(glm::vec3 direction, float dt)
{
	glm::vec3 dir(direction.x, direction.y, direction.z);
	m_Position += (dir * m_MoveSpeed * dt);
}

void Camera::Rotate(float dt_x, float dt_y)
{
	float sensitivity = 0.1f;

	m_Yaw += dt_x * m_RotSpeed * sensitivity;
	m_Pitch += dt_y * m_RotSpeed * sensitivity;


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

void Camera::SetPosition(glm::vec3 pos)
{
	m_Position = pos;
}



void Camera::Update()
{
	m_Front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front.y = sin(glm::radians(m_Pitch));
	m_Front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(m_Front);

	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
