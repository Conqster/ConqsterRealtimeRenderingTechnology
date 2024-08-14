#include "LearningOpenGLModel.h"

#include <GL/glew.h>
#include "External Libs/stb_image/stb_image.h"
#include "Graphics/Shader.h"

#include <iostream>

namespace learning {


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////MODEL LOADING////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	namespace model {

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////MESH///////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		L_Mesh::L_Mesh(std::vector<L_Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<L_Texture> _textures)
			:vertices(_vertices), indices(_indices), textures(_textures)
		{
			SetupMesh();
		}

		void L_Mesh::Draw(Shader& shader)
		{
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int emissiveNr = 1;

			for (unsigned int i = 0; i < textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);

				std::string number;
				std::string name = textures[i].type;

				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_emissive")
					number = std::to_string(emissiveNr++);

				shader.SetUniform1i(("u_Material." + name + number).c_str(), i);
				//std::cout << "Mat: " << "u_Matetial. " << name << number << "texture count: " << textures.size() << "\n";
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
			}
			glActiveTexture(GL_TEXTURE0);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		void L_Mesh::SetupMesh()
		{
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);

			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(L_Vertex), &vertices[0], GL_STATIC_DRAW);

			glGenBuffers(1, &IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			//POS
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(L_Vertex), (void*)0);
			//NOR
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(L_Vertex), (void*)offsetof(L_Vertex, normal));
			//TEX
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(L_Vertex), (void*)offsetof(L_Vertex, texCoord));

			glBindVertexArray(0);
		}


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////ASSIMP MODEL LOADING////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		void L_Model::Draw(Shader& shader)
		{
			for (auto& mesh : meshes)
				mesh.Draw(shader);
		}

		void L_Model::LoadModel(std::string path, bool flip_uv)
		{
			Assimp::Importer import;

			//aiPostProcssStep
			int process_step = aiProcess_Triangulate;

			if (flip_uv)
				process_step |= aiProcess_FlipUVs;

			const aiScene * scene = import.ReadFile(path, process_step);

			if (!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				std::cout << "[ASSIMP ERROR]:" << import.GetErrorString() << "\n";
				return;
			}

			directory = path.substr(0, path.find_last_of('/'));

			std::cout << directory << "\n";

			ProcessNode(scene->mRootNode, scene);
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
		void L_Model::ProcessNode(aiNode* node, const aiScene* scene)
		{
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(ProcessMesh(mesh, scene));
			}

			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				ProcessNode(node->mChildren[i], scene);
			}
		}

		L_Mesh L_Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
		{
			std::vector<L_Vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<L_Texture> textures;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				//Vertex vertex;
				//postion
				//normal 
				//texture coord

				aiVector3D vp = mesh->mVertices[i];
				aiVector3D n;
				if (mesh->HasNormals())
					n = mesh->mNormals[i];
				//aiVector3D t;
				glm::vec2 t;

				if (mesh->mTextureCoords[0])
				{
					t.x = mesh->mTextureCoords[0][i].x;
					t.y = mesh->mTextureCoords[0][i].y;
				}
				else
					t = glm::vec2(0.0f);


				L_Vertex vertex
				{
					glm::vec3(vp.x, vp.y, vp.z),
					glm::vec3(n.x, n.y, n.z),
					t,
				};
				vertices.push_back(vertex);
			}

			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			if (mesh->mMaterialIndex >= 0)
			{

				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				//int count = mesh->mMaterialIndex;
				//std::cout << "Material count" << count << "\n";

				//for (unsigned int type = aiTextureType_NONE; type <= aiTextureType_UNKNOWN; ++type)
				//	std::cout << material->GetTextureCount((aiTextureType)(type)) << ", type: " << type << "\n";


				std::vector<L_Texture> temp_diffuse_map = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
				textures.insert(textures.end(), temp_diffuse_map.begin(), temp_diffuse_map.end());

				std::vector<L_Texture> temp_specular_map = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
				textures.insert(textures.end(), temp_specular_map.begin(), temp_specular_map.end());

				std::vector<L_Texture> temp_emission_map = LoadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
				textures.insert(textures.end(), temp_emission_map.begin(), temp_emission_map.end());

				//std::vector<L_Texture> temp_metallic_map = LoadMaterialTextures(material, aiTextureType_METALNESS, "texture_specular");
				//textures.insert(textures.end(), temp_metallic_map.begin(), temp_metallic_map.end());
			}

			return L_Mesh(vertices, indices, textures);
		}

		std::vector<L_Texture> L_Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string type_name)
		{
			std::vector<L_Texture> temp_textures;

			//TO-DO: A function that checks the texture type.
			//		So i would be aware of material that i dont support yet
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString path;
				mat->GetTexture(type, i, &path);
				bool already_loaded = false;
#pragma region for optimization
				for (unsigned int j = 0; j < loadedTextures.size(); j++)
				{
					if (std::strcmp(loadedTextures[j].path.data(), path.C_Str()) == 0)
					{
						temp_textures.push_back(loadedTextures[j]);
						already_loaded = true;
						break;
					}
				}
#pragma endregion
				//Probably dont need this
				if (!already_loaded)
				{
					L_Texture texture;
					//std::string temp = path.C_Str();
					std::cout << "mat count for " << type_name << ": " << mat->GetTextureCount(type) << "\n";
					//if (temp.size() == 0)
					//	continue;

					texture.id = TextureFromFile(path.C_Str(), directory);
					//texture.id = TextureFromFile(path.C_Str(), directory);
					texture.type = type_name;
					texture.path = path.C_Str();
					temp_textures.push_back(texture);
					loadedTextures.push_back(texture);

					//return temp_textures;
				}
			}
			return temp_textures;
		}

		unsigned int TextureFromFile(const char* path, const std::string& dir, bool gamma)
		{
			std::string file_name = std::string(path);
			file_name = dir + '/' + file_name;

			std::cout << file_name << "\n";

			unsigned int textureID;
			glGenTextures(1, &textureID);

			int width,
				height,
				nrComponent;

			unsigned char* data = stbi_load(file_name.c_str(), &width, &height, &nrComponent, 0);
			if (data)
			{
				GLenum format;
				if (nrComponent == 1)
					format = GL_RED;
				else if (nrComponent == 3)
					format = GL_RGB;
				else if (nrComponent == 4)
					format = GL_RGBA;

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}
			else
			{
				std::cout << "[TEXTURE IMAGE LOADING]: failed to load texture at path " << path << "\n";
				stbi_image_free(data);
			}
			return textureID;



		}


	}



}


