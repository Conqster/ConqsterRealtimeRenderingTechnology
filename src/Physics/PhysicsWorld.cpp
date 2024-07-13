#include "PhysicsWorld.h"
#include "GameObject.h"
#include "glm/glm.hpp"

void PhysicsWorld::AddBody( GameObject* obj)
{
	m_Bodies.push_back(obj);
}

void PhysicsWorld::Update(float dt)
{
	for (auto& body : m_Bodies)
	{
		if (body->m_Static)
			continue;

		glm::vec3 weight = glm::vec3(0.0f, m_Gravity, 0.0f);
		//body->worldPos += weight * dt;
		//body->velocity += weight * dt;
		body->acceleration += weight;
	}


	CheckCollision();

	IntegrateVelocities(dt);
}

void PhysicsWorld::CheckCollision()
{
	float collision_dampling = 0.7f;

	for (size_t i = 0; i <= m_Bodies.size() - 1; i++)
	{
		for (size_t j = i + 1; j < m_Bodies.size(); j++)
		{
			//m_Bodies[0]->worldPos
			float dist = glm::distance(m_Bodies[i]->worldPos, m_Bodies[j]->worldPos);

			if (dist < (m_Bodies[i]->radius + m_Bodies[j]->radius))
			{
				if (!m_Bodies[i]->m_Static)
					m_Bodies[i]->velocity = -m_Bodies[i]->velocity * collision_dampling;
					////////////CAN BE DONE NEED A VELOCITY FOR ACCURATE / EASY CALCULATIOPN

				if(!m_Bodies[j]->m_Static)
					m_Bodies[j]->velocity = -m_Bodies[j]->velocity * collision_dampling;
			}
		}
	}
}

void PhysicsWorld::IntegrateVelocities(float dt)
{
	//glm::vec3 drag = glm::vec3();
	for (auto& body : m_Bodies)
	{
		body->velocity += body->acceleration * dt;
		body->worldPos += body->velocity * dt;
		body->acceleration = glm::vec3(0.0f);
	}
}
