#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <memory>
#include "Renderer/Meshes/Meshes.h"
#include "Scene Graph/Model.h"

#include "Scene Graph/Entity.h"

#include <tuple>

//Later be able to store texture paths/mesh data 
//without have to load to GPU memory
//Later check documentation if its possible to check is all other mesh ref same material


//I could still be using a model system but like a prefab. the my tech entity & renderer mesh would ref a model prefab
namespace CRRT
{
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

		unsigned int idx = 50;


		//Based on Assimp node-hierarchy
		//[Asset-Importer-Lib]:https://assimp-docs.readthedocs.io/en/latest/usage/use_the_lib.html#the-node-hierarchy
		void CopyNodesWithMesh(aiNode* node, const aiScene* scene, std::shared_ptr<Entity>& self_entity, glm::mat4 parent_trans);
		void CopyMeshes(aiNode* node, const aiScene* scene, std::shared_ptr<Entity> curr_entity);
		std::tuple<Mesh, Material> ProcessMesh2(aiMesh* mesh, const aiScene* scene);
		glm::mat4 ConvertMatrix(const aiMatrix4x4& from);



		/// <summary>
		/// There are better ways of calculating this 
		/// but for now it will do
		/// from [Foundation of Game Engine Developmenent - Rendering Eric Lengyel]
		/// </summary>
		void ForceCalculateTangents(aiMesh* mesh, std::vector<Vertex>& vertices);
	};


}