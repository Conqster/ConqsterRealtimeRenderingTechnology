#pragma once
#include <vector>


class PhysicsWorld
{
public:
	PhysicsWorld() = default;
	

	void AddBody(class GameObject* obj);

	void Update(float dt);


private:
	std::vector<GameObject*> m_Bodies;

	float m_Gravity = -9.8f;

	void CheckCollision();
	void IntegrateVelocities(float dt);
};
