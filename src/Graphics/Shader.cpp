#pragma once
#include "Shader.h"
#include "Renderer.h"

#include <fstream>
#include <iostream>


const char* LogShaderTypeName(GLenum shaderType)
{
	switch (shaderType)
	{
		case GL_VERTEX_SHADER : return "Vertex Shader";
		case GL_FRAGMENT_SHADER : return "Fragmant Shader";
	
		return "Not registed shader type";
	}
}

Shader::Shader()
	:m_ProgramID(0), m_VertShaderFilePath(""), 
	m_FragShaderFilePath(""), m_UniformLocationCache(std::unordered_map<std::string, int>())
{

}


bool Shader::CreateFromFile(const std::string& vShaderPath, const std::string& fShaderPath)
{
	m_VertShaderFilePath = vShaderPath;
	m_FragShaderFilePath = fShaderPath;

	std::string vertexString = ReadFile(vShaderPath);
	std::string fragmentString = ReadFile(fShaderPath);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	return CreateFromCode(vertexCode, fragmentCode);
}

bool Shader::CreateFromCode(const std::string& vshaderSrc, const std::string& fshaderSrc)
{
	m_ProgramID = glCreateProgram();

	GLuint vshader = CompileShader(GL_VERTEX_SHADER, vshaderSrc);
	GLuint fshader = CompileShader(GL_FRAGMENT_SHADER, fshaderSrc);

	GLCall(glAttachShader(m_ProgramID, vshader));
	GLCall(glAttachShader(m_ProgramID, fshader));

	glLinkProgram(m_ProgramID);
	glValidateProgram(m_ProgramID);


	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetProgramiv(m_ProgramID, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(m_ProgramID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return false;
	}

	//glUseProgram(program);
	GLCall(glDeleteShader(vshader));
	GLCall(glDeleteShader(fshader));

	SetupLights();

    return true;
}


void Shader::UseShader() const
{
	glUseProgram(m_ProgramID);
}

void Shader::ClearShader()
{
	if (m_ProgramID != 0)
	{
		glDeleteProgram(m_ProgramID);
		m_ProgramID = 0;
	}
}


void Shader::UsePointLight(std::vector<std::unique_ptr<PointLight>>& lights, unsigned int count)
{
	if (count > Shader_Constants::MAX_POINT_LIGHTS)
		count = Shader_Constants::MAX_POINT_LIGHTS;

	GLCall(glUniform1i(uniformPointLightCount, count));

	for (unsigned int i = 0; i < count; i++)
	{
		lights[i]->Use(m_UniformPointLightLocationCache[i].uniformColourLocation,
					m_UniformPointLightLocationCache[i].uniformAmbientIntensityLocation,
					m_UniformPointLightLocationCache[i].uniformDiffuseLocation,
					m_UniformPointLightLocationCache[i].uniformPositionLocation,
					m_UniformPointLightLocationCache[i].uniformAttenuationLocation);
	}

}

void Shader::UseSpotLight(std::vector<std::unique_ptr<SpotLight>>& lights, unsigned int count)
{
	if (count > Shader_Constants::MAX_SPOT_LIGHTS)
		count = Shader_Constants::MAX_SPOT_LIGHTS;

	GLCall(glUniform1i(uniformSpotLightCount, count));

	for (unsigned int i = 0; i < count; i++)
	{
		lights[i]->Use(m_UniformSpotLightLocationCache[i].uniformColourLocation,
			m_UniformSpotLightLocationCache[i].uniformAmbientIntensityLocation,
			m_UniformSpotLightLocationCache[i].uniformDiffuseLocation,
			m_UniformSpotLightLocationCache[i].uniformPositionLocation,
			m_UniformSpotLightLocationCache[i].uniformAttenuationLocation,
			m_UniformSpotLightLocationCache[i].uniformDirectionLocation,
			m_UniformSpotLightLocationCache[i].uniformFalloffLocation);
	}
}

void Shader::UseDirectionalLight(std::vector<std::unique_ptr<DirectionalLight>>& lights, unsigned int count)
{
	if (count > Shader_Constants::MAX_DIRECTIONAL_LIGHTS)
		count = Shader_Constants::MAX_DIRECTIONAL_LIGHTS;

	GLCall(glUniform1i(uniformDirectionalLightCount, count));

	for (unsigned int i = 0; i < count; i++)
	{
		lights[i]->Use(m_UniformDirectionalLightLocationCache[i].uniformColourLocation,
			m_UniformDirectionalLightLocationCache[i].uniformAmbientIntensityLocation,
			m_UniformDirectionalLightLocationCache[i].uniformDiffuseLocation,
			m_UniformDirectionalLightLocationCache[i].uniformDirectionLocation);
	}
}

void Shader::SetUniform1i(const char* name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const char* name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const char* name, const glm::mat4 matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::SetUniformVec3(const char* name, const glm::vec3 vector)
{
	GLCall(glUniform3f(GetUniformLocation(name), vector.x, vector.y, vector.z));
}

void Shader::SetUniform1f(const char* name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

int Shader::GetUniformLocation(const char* name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];


	GLCall(int location = glGetUniformLocation(m_ProgramID, name));
	if (location == -1)
		std::cout << "[Shader Warning]: uniform '" << name << "' doesn't exist!!!" << std::endl;
	
	m_UniformLocationCache[name] = location;
	return location;
}

Shader::~Shader()
{
	ClearShader();
}

unsigned int Shader::CompileShader(GLenum type, const std::string& source)
{
	GLuint shaderid = glCreateShader(type);

	const char* src = source.c_str();
	GLCall(glShaderSource(shaderid, 1, &src, nullptr));
	GLCall(glCompileShader(shaderid));


	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetShaderiv(shaderid, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		//TO-DO: Need to fix the debug message (for easy debugging)
		//current issue: misspelt a data type (sampler as sample) wrong error message
		//Error message was generic syntax error, unexpected IDENTIFIER, expecting
		// LEFT_BRACE or COMMA or SEMICOLON.
		glGetShaderInfoLog(shaderid, sizeof(eLog), NULL, eLog);
		printf("Could not create a %s, ERROR: %s", LogShaderTypeName(type), eLog);
		exit(-1);
	}

	return shaderid;
}


std::string Shader::ReadFile(const std::string& fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open())
	{
		printf("Failed to read %s! file doesn't exist.", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
		//std::cout << line << "\n";
	}


	fileStream.close();
	return content;
}

void Shader::SetupLights()
{
	uniformPointLightCount = GetUniformLocation("u_PointLightCount");
	for (size_t i = 0; i < Shader_Constants::MAX_POINT_LIGHTS; i++)
	{
		char colour_name[64];
		sprintf_s(colour_name, "u_PointLights[%zu].base.colour", i);
		m_UniformPointLightLocationCache[i].uniformColourLocation = glGetUniformLocation(m_ProgramID, colour_name);

		char ambient_name[64];
		sprintf_s(ambient_name, "u_PointLights[%zu].base.ambientIntensity", i);
		m_UniformPointLightLocationCache[i].uniformAmbientIntensityLocation = glGetUniformLocation(m_ProgramID, ambient_name);


		char diffuse_name[64];
		sprintf_s(diffuse_name, "u_PointLights[%zu].base.diffuseIntensity", i);
		m_UniformPointLightLocationCache[i].uniformDiffuseLocation = glGetUniformLocation(m_ProgramID, diffuse_name);

		char position_name[64];
		sprintf_s(position_name, "u_PointLights[%zu].position", i);
		m_UniformPointLightLocationCache[i].uniformPositionLocation = glGetUniformLocation(m_ProgramID, position_name);

		char attenuation_name[64];
		sprintf_s(attenuation_name, "u_PointLights[%zu].attenuation", i);
		m_UniformPointLightLocationCache[i].uniformAttenuationLocation = glGetUniformLocation(m_ProgramID, attenuation_name);
	}

	uniformSpotLightCount = GetUniformLocation("u_SpotLightCount");
	for (size_t i = 0; i < Shader_Constants::MAX_SPOT_LIGHTS; i++)
	{
		char colour_name[64];
		sprintf_s(colour_name, "u_SpotLights[%zu].pointLight.base.colour", i);
		m_UniformSpotLightLocationCache[i].uniformColourLocation = glGetUniformLocation(m_ProgramID, colour_name);

		char ambient_name[64];
		sprintf_s(ambient_name, "u_SpotLights[%zu].pointLight.base.ambientIntensity", i);
		m_UniformSpotLightLocationCache[i].uniformAmbientIntensityLocation = glGetUniformLocation(m_ProgramID, ambient_name);


		char diffuse_name[64];
		sprintf_s(diffuse_name, "u_SpotLights[%zu].pointLight.base.diffuseIntensity", i);
		m_UniformSpotLightLocationCache[i].uniformDiffuseLocation = glGetUniformLocation(m_ProgramID, diffuse_name);

		char position_name[64];
		sprintf_s(position_name, "u_SpotLights[%zu].pointLight.position", i);
		m_UniformSpotLightLocationCache[i].uniformPositionLocation = glGetUniformLocation(m_ProgramID, position_name);

		char attenuation_name[64];
		sprintf_s(attenuation_name, "u_SpotLights[%zu].pointLight.attenuation", i);
		m_UniformSpotLightLocationCache[i].uniformAttenuationLocation = glGetUniformLocation(m_ProgramID, attenuation_name);

		char direction_name[64];
		sprintf_s(direction_name, "u_SpotLights[%zu].direction", i);
		m_UniformSpotLightLocationCache[i].uniformDirectionLocation = glGetUniformLocation(m_ProgramID, direction_name);

		char falloff_name[64];
		sprintf_s(falloff_name, "u_SpotLights[%zu].falloff", i);
		m_UniformSpotLightLocationCache[i].uniformFalloffLocation = glGetUniformLocation(m_ProgramID, falloff_name);
	}

	uniformDirectionalLightCount = GetUniformLocation("u_DirectionalLightCount");
	for (size_t i = 0; i < Shader_Constants::MAX_DIRECTIONAL_LIGHTS; i++)
	{
		char colour_name[64];
		sprintf_s(colour_name, "u_DirectionalLights[%zu].base.colour", i);
		m_UniformDirectionalLightLocationCache[i].uniformColourLocation = glGetUniformLocation(m_ProgramID, colour_name);

		char ambient_name[64];
		sprintf_s(ambient_name, "u_DirectionalLights[%zu].base.ambientIntensity", i);
		m_UniformDirectionalLightLocationCache[i].uniformAmbientIntensityLocation = glGetUniformLocation(m_ProgramID, ambient_name);


		char diffuse_name[64];
		sprintf_s(diffuse_name, "u_DirectionalLights[%zu].base.diffuseIntensity", i);
		m_UniformDirectionalLightLocationCache[i].uniformDiffuseLocation = glGetUniformLocation(m_ProgramID, diffuse_name);

		char direction_name[64];
		sprintf_s(direction_name, "u_DirectionalLights[%zu].direction", i);
		m_UniformDirectionalLightLocationCache[i].uniformDirectionLocation = glGetUniformLocation(m_ProgramID, direction_name);
	}
}
