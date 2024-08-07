#pragma once

#include "glm/glm.hpp"

class Tranform
{
public:
	glm::vec3 position;
	glm::vec3 rotationAxis;
	glm::vec3 scale;
};


//forward declare haha
class Mesh;
class Texture;
//class Material;

//TRACKING THE SELECTED GAMEOBJECT IN RENDER AS INT 
//4 BYTES COMPARE TO GAMEOBJECT SIZE
class GameObject
{
private:
	//TO-DO: use aggregation 
	Mesh* m_Mesh = nullptr;
	Texture* m_Texture = nullptr;
	//Material* m_Material = nullptr;

	//std::shared_ptr<Material> m_Material;
	
public:
	//GameObject(Mesh& mesh);
	GameObject(Mesh* mesh, Texture* texture);
	//GameObject(Mesh* mesh, Texture* texture, Material* material);
	GameObject() = default;
	~GameObject();

	void Draw();
	void DebugDraw();

	void Cleanup();

	inline Texture* const GetTexture() const { return m_Texture; }
	inline Mesh* const GetMesh() const { return m_Mesh; }
	//inline Material* const GetMaterial() const { return m_Material; }

	bool selected = false;
	bool testImGui = false;

	//glm::vec3 worldPos = glm::vec3(0.0f);
	glm::vec3 worldPos = glm::vec3(0.0f);
	glm::vec4 rotation = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
	glm::vec3 worldScale = glm::vec3(1.0f);		

	bool m_Static = false;
	float radius = 1.0f;
	glm::vec3 acceleration = glm::vec3(0.0f);
	glm::vec3 velocity = glm::vec3(0.0f);
};