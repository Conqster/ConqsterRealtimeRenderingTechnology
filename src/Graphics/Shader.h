#pragma once
#include "GL/glew.h"
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>



struct ShaderFilePath
{
	const std::string& vertexPath = "";
	const std::string& fragmentPath = "";

	//special shaders 
	const std::string& geometryPath = "";
};


class Shader
{
private:
	unsigned int m_ProgramID = 0;
	//NewShaderFilePath m_ShaderFilePath{"", ""}; do not use
	std::string m_VertexFilePath = "";
	std::string m_FragFilePath = "";
	std::string m_GeometryFilePath = "";
	const char* m_Name = "";

	std::unordered_map<std::string, int> cacheUniformLocations = std::unordered_map<std::string, int>();
public:
	Shader() = default;

	bool Create(const char* name, const ShaderFilePath& file_paths);
	bool CreateFromFile(const ShaderFilePath& file_paths);

	void Bind() const;
	void UnBind() const;

	inline const char* GetName() const { return m_Name; }
	
	const char* GetShaderFilePath(GLenum shader_type);

	//TO-DO: Abstract out later
	void SetUniform1i(const char* name, int value);
	void SetUniform1f(const char* name, float value);
	void SetUniformVec3f(const char* name, const float* value);
	void SetUniform4f(const char* name, float v0, float v1, float v2, float v3);
	void SetUniformVec3(const char* name, const glm::vec3 vector);
	void SetUniformVec4(const char* name, const glm::vec4 vector);
	void SetUniformMat4f(const char* name, const glm::mat4 matrix);

	void Clear();
	~Shader();

private: 
	unsigned int CompileShader(GLenum shader_type, const std::string& source);
	std::string ReadFile(const std::string& shader_file);
	bool CreateFromCode(const char* vCode, const char* fCode, const std::string& gCode); //fix later, passing geometry as string to check .empty

	int GetUniformLocation(const char* name);
};
