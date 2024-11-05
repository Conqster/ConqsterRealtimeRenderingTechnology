#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <memory>
#include "Renderer/Meshes/Meshes.h"
#include "Scene Graph/Model.h"

#include "Scene Graph/Entity.h"

#include <tuple>

//const char* name = "default";
//int id = 0; //<-------- temp id
////later add a shader ref
//std::shared_ptr<Shader> shader = nullptr;
//
//glm::vec3 baseColour = glm::vec3(1.0f); //base coour contributes to base map (* multply as a mask)
//
//std::shared_ptr<Texture> baseMap = nullptr;
//std::shared_ptr<Texture> normalMap = nullptr;
//bool useParallax = false;
//std::shared_ptr<Texture> parallaxMap = nullptr;
//float heightScale = 0.0f; //parallax scale
//int shinness = 64;

//Later be able to store texture paths/mesh data 
//without have to load to GPU memory
//Later check documentation if its possible to check is all other mesh ref same material


//I could still be using a model system but like a prefab. the my tech entity & renderer mesh would ref a model prefab




namespace CRRT
{
	//struct Material;
	//struct Vertex;
	//class MeshDataObject
	//{
	//public:
	//	MeshDataObject();
	//	std::vector<Vertex> vertices;
	//	std::vector<unsigned int> indices;
	//	std::shared_ptr<Material> mat;

	//	std::shared_ptr<MeshDataObject> m_Parent;
	//	std::vector<std::shared_ptr<MeshDataObject>> m_Children;

	//	glm::mat4 m_Trans;

	//	void AddChild(std::shared_ptr<MeshDataObject> object)
	//	{
	//		m_Children.emplace_back(std::make_shared<MeshDataObject>(object));
	//		m_Chil
	//	}

	//};

	struct MeshVertex
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		//std::vector<Texture> textures;
	};



	class ModelLoader
	{
	public:
		ModelLoader() = default;

		std::shared_ptr<Model> Load(std::string path, bool flip_uv = false);
		std::shared_ptr<Entity> LoadAsEntity(std::string path, bool flip_uv = false);
	private:
		void Clean();

		//std::vector<Mesh>ProcessNode(aiNode* node, const aiScene* scene);
		std::vector<ModelMesh> ProcessNodes(aiNode* node, const aiScene* scene);
		ModelMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);

		std::string dir = "";
		std::vector<Texture> cacheLoadedTextures;

		unsigned int idx = 0;


		//Based on Assimp node-hierarchy
		//[Asset-Importer-Lib]:https://assimp-docs.readthedocs.io/en/latest/usage/use_the_lib.html#the-node-hierarchy
		void CopyNodesWithMesh(aiNode* node, const aiScene* scene, std::shared_ptr<Entity>& self_entity, glm::mat4 parent_trans);
		void CopyMeshes(aiNode* node, const aiScene* scene, std::shared_ptr<Entity> curr_entity);
		std::tuple<Mesh, Material> ProcessMesh2(aiMesh* mesh, const aiScene* scene);
		glm::mat4 ConvertMatrix(const aiMatrix4x4& from);

	};


}