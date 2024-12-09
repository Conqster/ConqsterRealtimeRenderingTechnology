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

struct ShaderBlockingIdx
{
	std::string name;
	unsigned int idx;

	//a compare operator to check two block
};


class Shader
{
private:
	unsigned int m_ProgramID = 0;
	//NewShaderFilePath m_ShaderFilePath{"", ""}; do not use
	std::string m_VertexFilePath = "";
	std::string m_FragFilePath = "";
	std::string m_GeometryFilePath = "";
	std::string m_Name = "";

	std::unordered_map<std::string, int> cacheUniformLocations = std::unordered_map<std::string, int>();
	std::vector<ShaderBlockingIdx> cacheBindingBlocks = std::vector<ShaderBlockingIdx>();
public:
	Shader() = default;

	bool Create(const std::string& name, const ShaderFilePath& file_paths);
	bool Create(const std::string& name,  const std::string& ver, const std::string& frag, const std::string& geo);
	bool CreateFromFile(const ShaderFilePath& file_paths);

	bool SoftCreate(const std::string& name, const std::string& ver, const std::string& frag, const std::string& geo);

	void Bind() const;
	void UnBind() const;

	inline const std::string GetName() const { return m_Name; }
	
	const char* GetShaderFilePath(GLenum shader_type);
	std::vector<ShaderBlockingIdx> GetBindingBlocks() const { return cacheBindingBlocks; }

	//TO-DO: Abstract out later
	//void SetUniformBlockIdx(const char* name, int blockBindingIdx = 0) const;
	void SetUniformBlockIdx(const char* name, int blockBindingIdx = 0);
	void SetUniform1i(const char* name, int value);
	void SetUniform1f(const char* name, float value);
	void SetUniformVec3f(const char* name, const float* value);
	void SetUniform4f(const char* name, float v0, float v1, float v2, float v3);
	void SetUniformVec3(const char* name, const glm::vec3 vector);
	void SetUniformVec2(const std::string& name, const glm::vec2 vector);
	void SetUniformVec4(const char* name, const glm::vec4 vector);
	void SetUniformMat4f(const char* name, const glm::mat4 matrix);

	void Clear();
	~Shader();

private: 
	/// <summary>
	/// To compile shader src code, hard makes stops program is fails
	/// </summary>
	/// <param name="shader_type"></param>
	/// <param name="source"></param>
	/// <param name="hard"></param>
	/// <returns></returns>
	unsigned int CompileShader(GLenum shader_type, const std::string& source, bool hard = true);
	std::string ReadFile(const std::string& shader_file);
	/// <summary>
	/// To compile & link shaders, hard makes stops program is fails
	/// </summary>
	/// <param name="vCode"></param>
	/// <param name="fCode"></param>
	/// <param name="gCode"></param>
	/// <param name="hard"></param>
	/// <returns></returns>
	bool CreateFromCode(unsigned int& ID, const char* vCode, const char* fCode, const std::string& gCode, bool hard = true); //fix later, passing geometry as string to check .empty

	int GetUniformLocation(const char* name);

	friend struct SceneSerialiser;
};
