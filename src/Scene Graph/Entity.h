#pragma once
#include "glm/glm.hpp"

#include <memory>


//forward declare
class Mesh;
struct Material;

class Entity
{
private:
	int m_ID = 0;
	const char* m_Name = "Default-Mesh";
	glm::mat4 m_Transform = glm::mat4(1.0f);
	std::shared_ptr<Mesh> m_Mesh = nullptr;
	std::shared_ptr<Material> m_Material = nullptr;

public:
	Entity() = default; //need to randomly generate a uuid
	//Entity() = delete;
	Entity(int id, const char* name = "Default-Mesh", glm::mat4 trans = glm::mat4(1.0f), std::shared_ptr<Mesh> mesh = nullptr, std::shared_ptr<Material> mat = nullptr) :
			   m_ID(id), m_Name(name), m_Transform(trans), m_Mesh(mesh), m_Material(mat){};

	//retrive
	inline const int GetID() const { return m_ID; }
	inline const const char* GetName() const { return m_Name; }
	inline const glm::mat4& GetTransform() const { return m_Transform; }
	inline const std::shared_ptr<Mesh>& GetMesh() const { return m_Mesh; }
	inline const std::shared_ptr<Material>& GetMaterial() const { return m_Material; }

	//set
	void SetTransform(const glm::mat4& transform) { m_Transform = transform; }
	//remove set ID when random uuid creation is supported
	void SetID(int id) { m_ID = id; }

	//func
	void Draw(class Shader& shader); //later pass in renderer/context, pass shader for now
	void Destroy();
};


