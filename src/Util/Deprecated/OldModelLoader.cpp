
#include "OldModelLoader.h"


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
	//(Quick Fix): bleeding mesh, when loader is used for multiple model
	meshes.clear();		

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

void ModelLoader::Clean()
{
	dir = "";

	for (auto& t : cacheLoadedTextures)
		t.DisActivate();

	for (auto& m : meshes)
		m.Clear();
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
	///////////////
	// LOG
	///////////////
	printf("[PROCESS NODE] Mesh count: %d from %s\n", node->mNumMeshes, dir.c_str());
	printf("[PROCESS NODE] Mesh children count: %d from %s\n", node->mNumChildren, dir.c_str());

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

	aiVector3D vp;     //position
	aiVector3D vn;	   //normal
	aiVector2D st;     //texture coord

	////////////////////////////
	// VERTICES
	////////////////////////////
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vp = mesh->mVertices[i];
		vn = aiVector3D();

		if (mesh->HasNormals())
			vn = mesh->mNormals[i];

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
			 {vp.x, vp.y, vp.z, 1.0f},    //layout 0 => pos
			{1.0f, 0.0f, 1.0f, 1.0f},	//layout 1 => col  //magenta for debugging
			{st.x, st.y},			   //layout 2 => uv
			{vn.x, vn.y, vn.z},       //layout 3 => nor
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
		{
			indices.push_back(face.mIndices[j]);
		}
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


	////////////////////////
	// (Quick Hack): if model doesnt have normal generate from face & its associate vertices
	////////////////////////
	if (!mesh->HasNormals())
	{
		bool generateWcFace = false;

		if (!generateWcFace)
			vertices = CalcAverageNormalsWcIndices(vertices, indices);
		else
			vertices = CalcNormalsWcMeshFace(vertices, mesh);
		
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


			tex.LoadTexture(file_repath, false, TextureFormat::RGBA, ConvertType(type));
			tex.RelativePath(path.C_Str());
			temp.push_back(tex);
			cacheLoadedTextures.push_back(tex);
		}

	}

	return temp;
}

std::vector<Vertex> ModelLoader::CalcAverageNormalsWcIndices(std::vector<Vertex>& vertices, std::vector<unsigned int> indices)
{
	std::vector<Vertex> temp = vertices;

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		unsigned int in0 = indices[i];    
		unsigned int in1 = indices[i + 1]; 
		unsigned int in2 = indices[i + 2]; 
	
		glm::vec3 v1;
		glm::vec3 v2;
	
		v1 = glm::vec3(temp[in1].position[0] - temp[in0].position[0], 
					   temp[in1].position[1] - temp[in0].position[1], 
					   temp[in1].position[2] - temp[in0].position[2]);
	
		v2 = glm::vec3(temp[in2].position[0] - temp[in0].position[0], 
					   temp[in2].position[1] - temp[in0].position[1], 
					   temp[in2].position[2] - temp[in0].position[2]);
		
		glm::vec3 nor = glm::cross(v1, v2);
		nor = glm::normalize(nor);
	
		//(accumulate) add current normally to all current vertices
		temp[in0].normals[0] += nor.x;
		temp[in0].normals[1] += nor.y;
		temp[in0].normals[2] += nor.z;
	
		temp[in1].normals[0] += nor.x;
		temp[in1].normals[1] += nor.y;
		temp[in1].normals[2] += nor.z;
	
		temp[in2].normals[0] += nor.x;
		temp[in2].normals[1] += nor.y;
		temp[in2].normals[2] += nor.z;
	}

	//re-normalizing
	for (size_t i = 0; i < temp.size(); i++)
	{
		glm::vec3 vec(temp[i].normals[0], temp[i].normals[1], temp[i].normals[2]);
		vec = glm::normalize(vec);

		if (glm::length(vec) > 0.0f)
			vec = glm::normalize(vec);

		temp[i].normals[0] = vec.x;
		temp[i].normals[1] = vec.y;
		temp[i].normals[2] = vec.z;
	}

	return temp;
}

std::vector<Vertex> ModelLoader::CalcNormalsWcMeshFace(std::vector<Vertex>& vertices, aiMesh* mesh)
{
	std::vector<Vertex> temp = vertices;

	aiVector3D v0;
	aiVector3D v1;
	aiVector3D v2;

	glm::vec3 n0;
	glm::vec3 n1;
	glm::vec3 n2;

	/////////v2///////// v21 = (v1 - v2)
	//////////#///////// v20 = (v0 - p2) 
	////////#//#//////// n2 = Cross(v21, v20)
	///////#////#///////
	//////#//////#//////
	/////#///F////#/////
	////#//////////#////
	///##############///
	//v0/////////////v1/

	aiVector3D v02;
	aiVector3D v01;
	aiVector3D v12;
	//aiVector3D v10; == -v01
	//aiVector3D v21; == -v12
	//aiVector3D v20; == -v02

	//Expensive but for testing 
	//std::vector<aiVector3D> cache_vectors;
	int count = 0;

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		if (face.mNumIndices > 0)
		{
			v0 = mesh->mVertices[face.mIndices[0]];
			v1 = mesh->mVertices[face.mIndices[1]];
			v2 = mesh->mVertices[face.mIndices[2]];
			

			//new
			v02 = v2 - v0;
			v01 = v1 - v0;
			v12 = v2 - v1;

			v02.Normalize();
			v01.Normalize();
			v12.Normalize();

			//n0
			n0 = glm::cross(glm::vec3(v02.x, v02.y, v02.z), glm::vec3(v01.x, v01.y, v01.z));
			//glm::normalize(n0);
			temp[face.mIndices[0]].normals[0] += n0.x;
			temp[face.mIndices[0]].normals[1] += n0.y;
			temp[face.mIndices[0]].normals[2] += n0.z;

			//n1
			n1 = glm::cross(glm::vec3(v12.x, v12.y, v12.z), glm::vec3(-v01.x, -v01.y, -v01.z));
			//glm::normalize(n1);
			temp[face.mIndices[1]].normals[0] += n1.x;
			temp[face.mIndices[1]].normals[1] += n1.y;
			temp[face.mIndices[1]].normals[2] += n1.z;

			//n2
			n2 = glm::cross(glm::vec3(-v12.x, -v12.y, -v12.z), glm::vec3(-v02.x, -v02.y, -v02.z));
			//glm::normalize(n2);
			temp[face.mIndices[2]].normals[0] += n2.x;
			temp[face.mIndices[2]].normals[1] += n2.y;
			temp[face.mIndices[2]].normals[2] += n2.z;
		}

	}

	return temp;
}


