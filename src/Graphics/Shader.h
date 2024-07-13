#pragma once
#include "GL/glew.h"
#include <string>

#include <unordered_map>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "Util/Constants.h"
#include "Lights/SpotLight.h"
#include "Lights/DirectionalLight.h"


class Shader
{
private:
	unsigned int m_ProgramID;
	std::string m_VertShaderFilePath;
	std::string m_FragShaderFilePath;

	std::unordered_map<std::string, int> m_UniformLocationCache;


	struct uniformPointLight
	{
		int uniformColourLocation;
		int uniformAmbientIntensityLocation;
		int uniformDiffuseLocation;

		int uniformPositionLocation;
		int uniformAttenuationLocation;
	};
	unsigned int uniformPointLightCount;
	uniformPointLight m_UniformPointLightLocationCache[Shader_Constants::MAX_POINT_LIGHTS];


	struct uniformSpotLight
	{
		int uniformColourLocation;
		int uniformAmbientIntensityLocation;
		int uniformDiffuseLocation;

		int uniformPositionLocation;
		int uniformAttenuationLocation;

		int uniformDirectionLocation;
		int uniformFalloffLocation;
	};
	unsigned int uniformSpotLightCount;
	uniformSpotLight m_UniformSpotLightLocationCache[Shader_Constants::MAX_SPOT_LIGHTS];

	struct uniformDirectionalLight
	{
		int uniformColourLocation;
		int uniformAmbientIntensityLocation;
		int uniformDiffuseLocation;

		int uniformDirectionLocation;
	};
	unsigned int uniformDirectionalLightCount;
	uniformDirectionalLight m_UniformDirectionalLightLocationCache[Shader_Constants::MAX_DIRECTIONAL_LIGHTS];

public:
	Shader();
	//Shader(const std::string& vshaderSrc, const std::string& fshaderSrc);
	//Shader(const std::string& vShaderPath, const std::string& fShaderPath);

	bool CreateFromCode(const std::string& vshaderSrc, const std::string& fshaderSrc);
	bool CreateFromFile(const std::string& vShaderPath, const std::string& fShaderPath);

	void UseShader() const;
	void ClearShader();


	void SetupLights();
	//Might change to updateLight
	void UsePointLight(std::vector<std::unique_ptr<PointLight>>& lights, unsigned int count);
	void UseSpotLight(std::vector<std::unique_ptr<SpotLight>>& lights, unsigned int count);
	void UseDirectionalLight(std::vector<std::unique_ptr<DirectionalLight>>& lights, unsigned int count);

	void SetUniform1i(const char* name, int value);
	void SetUniform4f(const char* name, float v0, float v1, float v2, float v3);
	void SetUniformVec3(const char* name, const glm::vec3 vector);
	void SetUniformMat4f(const char* name, const glm::mat4 matrix);
	void SetUniform1f(const char* name, float value);
	int GetUniformLocation(const char* name);

	~Shader();
private:
	unsigned int CompileShader(GLenum type, const std::string& source);
	std::string ReadFile(const std::string& fileLocation);

};