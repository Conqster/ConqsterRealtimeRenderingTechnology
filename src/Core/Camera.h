#pragma once
#include "Util/Vector.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	~Camera();
	Camera(glm::vec3 startPosition, glm::vec3 worldUp, float startYaw, float startPitch, float startMoveSpeed, float startRotSpeed);

	void SetPosition(glm::vec3 pos);
	void Translate(glm::vec3 direction, float dt);
	void Rotate(float dt_x, float dt_y);

	glm::mat4 CalViewMat();
	glm::mat4 CalculateProjMatrix(float aspect_ratio) const;

	inline glm::vec3 const GetPosition() { return m_Position; } 
	inline glm::vec3 const GetForward() {return glm::normalize(m_Front);} 
	inline glm::vec3 const GetRight() { return glm::normalize(m_Right); } 
	inline glm::vec3 const GetUp() { return glm::normalize(m_Up); } 


	inline float* const Ptr_FOV() { return &m_FOV; }
	inline float* const Ptr_Near() { return &m_Near; }
	inline float* const Ptr_Far() { return &m_Far; }
	inline float* const Ptr_MoveSpeed() { return &m_MoveSpeed; }
	inline float* const Ptr_RotSpeed() { return &m_RotSpeed; } 
	inline float* const Ptr_Pitch() { return &m_Pitch; }
	inline float* const Ptr_Yaw() { return &m_Yaw; }

private:
	void Update();

	glm::vec3 m_Position = glm::vec3(0.0f);;
	glm::vec3 m_WorldUp;

	float m_Yaw = 90.0f;
	float m_Pitch = 0.0f;

	float m_MoveSpeed = 100.0f;
	float m_RotSpeed = 10.0f;

	//coordinate forward
	glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_Up = glm::vec3(0.0f);
	glm::vec3 m_Right = glm::vec3(0.0f);

	float m_FOV = 60.0f;
	float m_Near = 0.1f;
	float m_Far = 150.0f;

	glm::vec3 m_InitialMousePos = glm::vec3(0.0f);

};