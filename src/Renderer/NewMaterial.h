#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace graphics {


	struct Vertex
	{
		glm::vec3 vertices;
		glm::vec3 position;
		glm::vec3 texCoord;
	};

	enum TextureType
	{
		TextureType_DIFFUSE,
		TextureType_SPECULAR,
		TextureType_EMISSIVE,
	};

	struct Texture
	{	
		uint16_t id;
		const char* name;
		const char* path;

		TextureType type;
	};

	struct Shader
	{
		uint16_t m_ProgramID;
		std::string v_filePath;
		std::string f_filePath;

		Shader(std::string vfilePath, std::string effilePath);

		void Bind();
		void UnBind();

		uint16_t CompileShader(std::string shader_type, const std::string& source);
		std::string ReadFile(const std::string& shader_file);
	};

	struct Material
	{
		//Material(Shader* shader);
		Material();

		std::vector<Texture*> m_Textures;

		Shader* m_Shader;
		glm::vec3 m_DiffuseColour;
		glm::vec3 m_SpecularColour;
		glm::vec3 m_EmissiveColour;

		void SetTexture(Texture* texture);
	};

	struct Mesh
	{
		std::vector<Vertex> m_vertices;
		std::vector<uint16_t> m_indices;

		Material m_Material;
		Mesh(std::vector<Vertex> _vertices, std::vector<uint16_t> _indices, std::vector<Texture> _textures);
		void Draw();
	};


	struct Gameobject
	{
		Gameobject(std::vector<Mesh> mesh);

		std::vector<Mesh> m_Meshes;

		glm::vec3 worldPos = glm::vec3(0.0f);
		glm::vec4 rotation = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
		glm::vec3 worldScale = glm::vec3(1.0f);


		void Draw();
		void Draw(Shader& shader);
	};

}