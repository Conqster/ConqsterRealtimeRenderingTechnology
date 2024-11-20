#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Scene Graph/Deprecated/Model.h"

#include <memory>

/// <summary>
/// Deprecated Class, use CRRT::ModelLoader (src/Util/ModelLoader.h)
/// </summary>
class ModelLoader
{
public:
	ModelLoader() = default;

	std::shared_ptr<Model> Load(std::string path, bool flip_uv = false);

	void Clean();
	~ModelLoader() = default;
private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	ModelMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);

	std::string dir = "";
	std::vector<Texture> cacheLoadedTextures;

	//store retrived data
	std::vector<ModelMesh> meshes = std::vector<ModelMesh>();


	//Helpers
	std::vector<Vertex> CalcAverageNormalsWcIndices(std::vector<Vertex>& vertices, std::vector<unsigned int> indices);
	std::vector<Vertex> CalcNormalsWcMeshFace(std::vector<Vertex>& vertices, aiMesh* mesh);
};