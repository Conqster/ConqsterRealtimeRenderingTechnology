

#include "ModelLoader.h"


TextureType ConvertType(aiTextureType ai_type)
{
	switch (ai_type)
	{
	case aiTextureType_NONE:
		return TextureType_NONE;
		break;
	case aiTextureType_DIFFUSE:
		return TextureType_DIFFUSE;
		break;
	case aiTextureType_SPECULAR:
		return TextureType_SPECULAR;
		break;
	case aiTextureType_EMISSIVE:
		return TextureType_EMISSIVE;
		break;
	}
}


std::shared_ptr<Model> ModelLoader::Load(std::string path, bool flip_uv)
{
	Assimp::Importer import; 

	int process_step = aiProcess_Triangulate;

	if (flip_uv)
		process_step |= aiProcess_FlipUVs;

	const aiScene* scene = import.ReadFile(path, process_step);

	if (!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf("[ASSIMP ERROR]: %s\n", import.GetErrorString());
		return nullptr;
	}

	dir = path.substr(0, path.find_last_of('/'));

	printf("%s\n", dir.c_str());

	ProcessNode(scene->mRootNode, scene);

	return std::make_shared<Model>(meshes);
}


/// <summary>
/// Process Node, processes starting from the root node:
/// 1: Check number of meshes in node, if any process each mesh
/// 2: After processing the meshes in node, check for child nodes.
/// 3: If any children node, Process each children by recursion.
/// 4: for each children and if children of children would repeat step 1 to 3.
/// </summary>
/// <param name="node"></param>
/// <param name="scene"></param>
void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene)
{

	//process all node in current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	//transverse through current nodes children (recursively)
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}


ModelMesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	//data from current mesh
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	////////////////////////////
	// VERTICES
	////////////////////////////
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		//Vertex vertex;
		//postion vp
		//normal  vn
		//texture coord st


		aiVector3D vp = mesh->mVertices[i];
		aiVector3D vn = aiVector3D();

		if (mesh->HasNormals())
			vn = mesh->mNormals[i];

		aiVector2D st;

		if (mesh->mTextureCoords[0])
		{
			st.x = mesh->mTextureCoords[0][i].x;
			st.y = mesh->mTextureCoords[0][i].y;
		}
		else
			st = aiVector2D();


		//{ SET UP
		//	float position[4];
		//	float colour[4];
		//	float texCoord[2];
		//	float normals[3];
		//}

		Vertex vertex
		{
			{vp.x, vp.y, vp.z, 1.0f},     //layout 1 => pos
			{1.0f, 0.0f, 1.0f, 1.0f}, //magenta   //layout 2 => col
			{st.x, st.y},				//layout 2 => uv
			{vn.x, vn.y, vn.z},         //layout 3 => nor
		};
		vertices.push_back(vertex);
	}


	////////////////////////////
	// INDICES
	////////////////////////////
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}



	////////////////////////////
	// To-Do: TEXTURE MATERIAL
	////////////////////////////
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuse_map = LoadMaterialTextures(mat, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuse_map.begin(), diffuse_map.end());

		std::vector<Texture> specular_map = LoadMaterialTextures(mat, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specular_map.begin(), specular_map.end());

		std::vector<Texture> emission_map = LoadMaterialTextures(mat, aiTextureType_EMISSIVE);
		textures.insert(textures.end(), emission_map.begin(), emission_map.end());

	}

	//Generate Necessary data for Mesh
	ModelMesh _mesh;
	_mesh.Generate(vertices, indices, textures);

	return _mesh;
}

std::vector<Texture> ModelLoader::LoadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
	std::vector<Texture> temp;


	//TO-DO: A function that checks the texture type.
	//		So i would be aware of material that i dont support yet
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString path;
		mat->GetTexture(type, i, &path);
		bool alrealdy_load = false;
#pragma region for optimisation
		for (unsigned int j = 0; j < cacheLoadedTextures.size(); j++)
		{
			if (std::strcmp(cacheLoadedTextures[j].GetRelativePath().data(), path.C_Str()) == 0)
			{
				temp.push_back(cacheLoadedTextures[j]);
				alrealdy_load = true;
				break;
			}
		}
#pragma endregion

		if (!alrealdy_load)
		{
			Texture tex;

			//to-do: fix file path dir extraction
			std::string file_repath = dir + '/' + path.C_Str();


			tex.LoadTexture(file_repath, ConvertType(type));
			tex.RelativePath(path.C_Str());
			temp.push_back(tex);
			cacheLoadedTextures.push_back(tex);
		}

	}

	return temp;
}
