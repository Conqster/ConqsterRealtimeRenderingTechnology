#pragma once
#include "Util/Vector.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera() = default;
	~Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	inline glm::vec3 const GetPosition() { return m_Position; } 
	inline glm::vec3 const GetForward() {return glm::normalize(m_Front);} 
	inline glm::vec3 const GetRight() { return glm::normalize(m_Right); } 
	inline glm::vec3 const GetUp() { return glm::normalize(m_Up); } 

	glm::mat4 CalViewMat();
	glm::mat4 CalculateProjMatrix(float aspect_ratio) const;

	inline float* const Ptr_FOV() { return &m_FOV; }
	inline float* const Ptr_Near() { return &m_Near; }
	inline float* const Ptr_Far() { return &m_Far; }

	void Translate(glm::vec3 direction, float dt);
	void Rotate(float dt_yaw, float dt_pitch);
	void Rotate(glm::vec2 mouse_pos, float height, float width);
	void NewRotate(glm::vec2 mouse_pos);

	void SetPosition(glm::vec3 pos);

	inline float* const Ptr_MoveSpeed() { return &m_MoveSpeed; }
	inline float* const Ptr_RotSpeed() { return &m_RotSpeed; } 
	//This is wrong but fix later 
	inline float const Ptr_Pitch() const { return m_Pitch; }
	inline float const Ptr_Yaw() const { return m_Yaw; }

	inline float* New_Yaw() { return &m_Yaw; }
	inline float* New_Pitch() { return &m_Pitch; }

private:

	void Update();

	glm::vec3 m_Position;
	glm::vec3 m_WorldUp;

	GLfloat m_Yaw;
	GLfloat m_Pitch;


	GLfloat m_MoveSpeed;
	GLfloat m_RotSpeed;

	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;

	float m_FOV = 60.0f;
	float m_Near = 0.1f;
	float m_Far = 150.0f;

	glm::vec3 m_InitialMousePos = glm::vec3(0.0f);

};