#include "ModelLoaderN.h"

#include "Renderer/Material.h"
#include "Scene Graph/Entity.h"

#include "Renderer/Meshes/Mesh.h"
#include "Renderer/Texture.h"

#include <tuple>

namespace CRRT
{


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


	/// <summary>
	/// Mesh Renderable Data
	/// </summary>
	/// <param name="path"></param>
	/// <param name="flip_uv"></param>
	/// <returns></returns>
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

		std::vector<ModelMesh> model_meshes = ProcessNodes(scene->mRootNode, scene);
		return std::make_shared<Model>(model_meshes);
		//return std::make_shared<RetrievedRenderData>(meshes);
	}

	std::shared_ptr<Entity> ModelLoader::LoadAsEntity(std::string path, bool flip_uv)
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

		auto root_entity = std::make_shared<Entity>(idx++, "root Entity");
		CopyNodesWithMesh(scene->mRootNode, scene, root_entity, glm::mat4(1.0f));
		//ProcessMeshData(scene->mRootNode, scene, root_entity);
		return root_entity;
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
	std::vector<ModelMesh> ModelLoader::ProcessNodes(aiNode* node, const aiScene* scene)
	{
		///////////////
		// LOG
		///////////////
		printf("[PROCESS NODE] Mesh count: %d from %s\n", node->mNumMeshes, dir.c_str());
		printf("[PROCESS NODE] Mesh children count: %d from %s\n", node->mNumChildren, dir.c_str());

		std::vector<ModelMesh> temp_mesh; 

		//process all node in current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			temp_mesh.push_back(ProcessMesh(mesh, scene));
		}

		//transverse through current nodes children (recursively)
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			std::vector<ModelMesh> temp = ProcessNodes(node->mChildren[i], scene);
			temp_mesh.insert(temp_mesh.end(), temp.begin(), temp.end());
		}
		return temp_mesh;
	}


	ModelMesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		//data from current mesh
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		aiVector3D vp;     //position
		aiVector3D vn;	   //normal
		aiVector2D st;     //texture coord

		//Later
		//glm::vec3 tn;   //tangent 
		//glm::vec3 bt;	  //bitangent

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
		//Later check documentation if its possible to check is all other mesh ref same material
		std::shared_ptr<Material> new_mat = nullptr;
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];


			std::vector<Texture> diffuse_map = LoadMaterialTextures(mat, aiTextureType_DIFFUSE);
			//textures.insert(textures.end(), diffuse_map.begin(), diffuse_map.end());
			std::vector<Texture> specular_map = LoadMaterialTextures(mat, aiTextureType_SPECULAR);
			//textures.insert(textures.end(), specular_map.begin(), specular_map.end());
			std::vector<Texture> emission_map = LoadMaterialTextures(mat, aiTextureType_EMISSIVE);
			//textures.insert(textures.end(), emission_map.begin(), emission_map.end());
			
			//For now only use 
			//baseMap >> diffuseMap
			//normalMap>>
			//parallaxMap 
			std::vector<Texture> normal_map = LoadMaterialTextures(mat, aiTextureType_NORMALS);
			std::vector<Texture> parallax_map = LoadMaterialTextures(mat, aiTextureType_HEIGHT);

			if (diffuse_map.size() > 0)
				new_mat->baseMap = std::make_shared<Texture>(diffuse_map[0]);
			if (normal_map.size() > 0)
				new_mat->normalMap = std::make_shared<Texture>(normal_map[0]);
			if (parallax_map.size() > 0)
				new_mat->parallaxMap = std::make_shared<Texture>(parallax_map[0]);
		}


		////////////////////////
		// (Quick Hack): if model doesnt have normal generate from face & its associate vertices
		////////////////////////
		//if (!mesh->HasNormals())
		//{
		//	bool generateWcFace = false;

		//	if (!generateWcFace)
		//		vertices = CalcAverageNormalsWcIndices(vertices, indices);
		//	else
		//		vertices = CalcNormalsWcMeshFace(vertices, mesh);

		//}
		//Generate Necessary data for Mesh
		ModelMesh _mesh;
		_mesh.Generate(vertices, indices, new_mat);

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

	glm::mat4 ModelLoader::ConvertMatrix(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		to[0][0] = from.a1; to[0][1] = from.b1; to[0][2] = from.c1; to[0][3] = from.d1;
		to[1][0] = from.a2; to[1][1] = from.b2; to[1][2] = from.c2; to[1][3] = from.d2;
		to[2][0] = from.a3; to[2][1] = from.b3; to[2][2] = from.c3; to[2][3] = from.d3;
		to[3][0] = from.a4; to[3][1] = from.b4; to[3][2] = from.c4; to[3][3] = from.d4;
		return to;
	}

	std::tuple<Mesh, Material> ModelLoader::ProcessMesh2(aiMesh* mesh, const aiScene* scene)
	{
		//data from current mesh
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		aiVector3D vp;     //position
		aiVector3D vn;	   //normal
		aiVector2D st;     //texture coord

		//Later
		//glm::vec3 tn;   //tangent 
		//glm::vec3 bt;	  //bitangent

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
		//Later check documentation if its possible to check is all other mesh ref same material
		Material gen_mat;
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];


			std::vector<Texture> diffuse_map = LoadMaterialTextures(mat, aiTextureType_DIFFUSE);
			//textures.insert(textures.end(), diffuse_map.begin(), diffuse_map.end());
			std::vector<Texture> specular_map = LoadMaterialTextures(mat, aiTextureType_SPECULAR);
			//textures.insert(textures.end(), specular_map.begin(), specular_map.end());
			std::vector<Texture> emission_map = LoadMaterialTextures(mat, aiTextureType_EMISSIVE);
			//textures.insert(textures.end(), emission_map.begin(), emission_map.end());

			//For now only use 
			//baseMap >> diffuseMap
			//normalMap>>
			//parallaxMap 
			std::vector<Texture> normal_map = LoadMaterialTextures(mat, aiTextureType_NORMALS);
			std::vector<Texture> parallax_map = LoadMaterialTextures(mat, aiTextureType_HEIGHT);

			if (diffuse_map.size() > 0)
				gen_mat.baseMap = std::make_shared<Texture>(diffuse_map[0]);
			if (normal_map.size() > 0)
				gen_mat.normalMap = std::make_shared<Texture>(normal_map[0]);
			if (parallax_map.size() > 0)
				gen_mat.parallaxMap = std::make_shared<Texture>(parallax_map[0]);
		}


		////////////////////////
		// (Quick Hack): if model doesnt have normal generate from face & its associate vertices
		////////////////////////
		//if (!mesh->HasNormals())
		//{
		//	bool generateWcFace = false;

		//	if (!generateWcFace)
		//		vertices = CalcAverageNormalsWcIndices(vertices, indices);
		//	else
		//		vertices = CalcNormalsWcMeshFace(vertices, mesh);

		//}
		//Generate Necessary data for Mesh
		ModelMesh _mesh;
		//_mesh.Generate(vertices, indices, new_mat);
		Mesh gen_mesh;
		gen_mesh.Generate(vertices, indices);

		return std::make_tuple(gen_mesh, gen_mat);
	}


	void ModelLoader::CopyNodesWithMesh(aiNode* node, const aiScene* scene, std::shared_ptr<Entity>& self_entity, glm::mat4 parent_trans)
	{
		glm::mat4 node_trans = glm::mat4(1.0f);
		aiMatrix4x4 assimp_trans = node->mTransformation;


		if (node->mNumMeshes > 0)
			CopyMeshes(node, scene, self_entity);

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			node_trans = ConvertMatrix(assimp_trans);
			glm::mat4 world_trans = parent_trans * node_trans;
			std::shared_ptr<Entity> curr_entity = std::make_shared<Entity>(idx++, node->mChildren[i]->mName.C_Str(), node_trans);
			self_entity->AddLocalChild(curr_entity);
			//self_entity->AddWorldChild(curr_entity);
			CopyNodesWithMesh(node->mChildren[i], scene, curr_entity, world_trans);
		}

	}

	void ModelLoader::CopyMeshes(aiNode* node, const aiScene* scene, std::shared_ptr<Entity> curr_entity)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			std::tuple<Mesh, Material> process_mesh = ProcessMesh2(mesh, scene);
			//mesh data
			std::shared_ptr<Mesh> entity_mesh = std::make_shared<Mesh>(std::get<0>(process_mesh));
			//update important data
			std::shared_ptr<Material> entity_mat = std::make_shared<Material>(std::get<1>(process_mesh));

			curr_entity->SetMesh(entity_mesh);
			curr_entity->SetMaterial(entity_mat);
		}
	}

}