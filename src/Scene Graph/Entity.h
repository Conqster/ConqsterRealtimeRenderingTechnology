#pragma once
#include "glm/glm.hpp"

#include <memory>
#include <vector>

#include <string>

//forward declare
class Mesh;
struct Material;

class Entity : public std::enable_shared_from_this<Entity>
{
private:
	int m_ID = 0;
	std::string m_Name = "Default-Mesh";
	glm::mat4 m_LocalTransform = glm::mat4(1.0f);
	glm::mat4 m_WorldTransform = glm::mat4(1.0f);
	bool m_DirtyTranform = false;
	std::shared_ptr<Mesh> m_Mesh = nullptr;
	std::shared_ptr<Material> m_Material = nullptr;

	//To-Do: fix later as we probably only need the parents ID,
	//and transform to determine this world transform
	std::shared_ptr<Entity> m_Parent = nullptr;
	std::vector<std::shared_ptr<Entity>> m_Children;

	void UpdateWorldTransform();
	void MarkTransformDirty();
public:
	Entity() = default; //need to randomly generate a uuid
	//Entity() = delete;
	Entity(int id, std::string name = "Default-Mesh", glm::mat4 trans = glm::mat4(1.0f), std::shared_ptr<Mesh> mesh = nullptr, std::shared_ptr<Material> mat = nullptr) :
		m_ID(id), m_Name(name), m_LocalTransform(trans), m_WorldTransform(trans), m_Mesh(mesh), m_Material(mat) {
		MarkTransformDirty();
	};

	std::shared_ptr<Entity> GetRef() { return shared_from_this(); }

	//retrive
	inline const int GetID() const { return m_ID; }
	inline const const char* GetName() const { return m_Name.c_str(); }
	const glm::mat4& GetWorldTransform();
	inline const glm::mat4& GetTransform() const { return m_LocalTransform; }
	inline const std::shared_ptr<Mesh>& GetMesh() const { return m_Mesh; }
	inline const std::shared_ptr<Material>& GetMaterial() const { return m_Material; }
	inline const std::vector<std::shared_ptr<Entity>>& GetChildren() const { return m_Children; }
	inline const std::shared_ptr<Entity>& GetParent() const { return m_Parent; }

	//set
	void SetTransform(const glm::mat4& transform) 
	{ 
		m_LocalTransform = transform; 
		MarkTransformDirty();
	}
	//remove set ID when random uuid creation is supported
	void SetID(int id) { m_ID = id; }

	//func
	void AddLocalChild(const Entity& entity);
	void AddWorldChild(const Entity& entity);
	void SetParent(const std::shared_ptr<Entity> p) { m_Parent = p; }
	void Draw(class Shader& shader); //later pass in renderer/context, pass shader for now
	void Destroy();

	void SetMaterial(std::shared_ptr<Material>& mat) { m_Material = mat; }
	void SetMesh(std::shared_ptr<Mesh>& mesh) { m_Mesh = mesh; }
	void AddLocalChild(const std::shared_ptr<Entity>& entity);
	void AddWorldChild(const std::shared_ptr<Entity>& entity);
};

