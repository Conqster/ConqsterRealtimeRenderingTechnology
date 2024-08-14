#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Graphics/Shader.h"



namespace learning {


	struct ObjectVertex
	{
		unsigned int VAO;
		unsigned int VBO;
	};


	struct L_Material
	{
		/*Good setup for Materials
		* 3 maps :- Diffuse, Specular & Emssion
		* shiniess value of specular
		* strength value of emission
		* Colour of Emission
		*/
		int diffuseMap;
		int specularMap;
		int emissionMap;
		/*float*/ int shiniess;

		//stored as int foe simplity
		bool invertSpecularMap;
		glm::vec3 emissionColour;
		float emissionStrength;
		bool emit;
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////MODEL LOADING////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	namespace light {

		struct L_Light
		{
			/*Still trying try to decide what
			* is best for light, FOR:
			* FOR:- Ambient, diffuse & Specular
			* To have seperate colour (vec3) OR
			* TO only intensity that influence a single colour
			*/
			//glm::vec3 position;

			glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
			glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);

			bool use = false;
		};


		struct L_DirectionLight
		{
			/* Up for debate
			* Directional Light could inherit from Light
			* Or as light as a memeber (Aggergation)
			*/
			L_Light lightBase;
			glm::vec3 direction;
		};


		struct L_PointLight
		{
			L_Light lightBase;

			glm::vec3 position = glm::vec3(0.0f);

			float constant = 1.0f;     //con
			float linear = 0.09f;    //lin
			float quadratic = 0.032f;   //quad
		};

		struct L_SpotLight
		{
			L_PointLight pointLightBase;

			glm::vec3 direction;
			float innerCutoffAngle;
			float outerCutoffAngle;
		};

	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////MODEL LOADING////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	namespace model {


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////MESH PROPERTIES(TEXTURE/VERTEX)////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		enum L_TextureType
		{
			TextureType_DIFFUSE,
			TextureType_SPECULAR,
		};


		struct L_Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texCoord;
		};


		struct L_Texture
		{
			unsigned int id;
			std::string type;

			std::string path;

			L_TextureType tex_type;
		};


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////MESH///////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		class L_Mesh
		{
		public:
			std::vector<L_Vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<L_Texture> textures;

			L_Mesh(std::vector<L_Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<L_Texture> _textures);
			void Draw(class Shader& shader);
		private:
			//render data 
			unsigned int VAO, VBO, IBO;
			void SetupMesh();
		};






		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////ASSIMP MODEL LOADING////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//Helper
		unsigned int TextureFromFile(const char* path, const std::string& dir, bool gamma = false);

		class L_Model
		{
		public:
			L_Model() = default;
			L_Model(char* path, bool flip_uv = false)
			{
				LoadModel(path, flip_uv);
			}

			void GenModel(const char* path, bool flip_uv = false)
			{
				LoadModel(path, flip_uv);
			}

			void Draw(class Shader& shader);

		private:
			//model data
			std::vector<L_Mesh> meshes;
			std::vector<L_Texture> loadedTextures;
			std::string directory;

			void LoadModel(std::string path, bool flip_uv = false);
			void ProcessNode(aiNode* node, const aiScene* scene);
			L_Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
			std::vector<L_Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string type_name);
		};


	}




}

