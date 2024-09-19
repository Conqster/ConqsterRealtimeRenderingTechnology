#pragma once
#include <vector>

#include "Graphics/Meshes/ModelMesh.h"
#include "Graphics/Texture.h"

class Model
{
public:
	Model() = default;
	Model(std::vector<ModelMesh> _meshes);

	void Draw();
	void Draw(class Shader& shader);

	void Destroy();
private: 
	std::vector<ModelMesh> meshes;
	std::vector<Texture> textures;

	std::string dir;
};