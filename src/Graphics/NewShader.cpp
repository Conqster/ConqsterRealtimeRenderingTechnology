#pragma once
#include "NewShader.h"
#include <fstream>

#include "RendererErrorAssertion.h"

	const char* LogShaderTypeName2(GLenum shaderType)
	{
		switch (shaderType)
		{
		case GL_VERTEX_SHADER: return "Vertex Shader";
		case GL_FRAGMENT_SHADER: return "Fragmant Shader";

			return "Not registed shader type";
		}
	}


	bool NewShader::Create(const char* name, const NewShaderFilePath& file_paths)
	{
		m_Name = name;
		m_FragFilePath = file_paths.fragmentPath;
		m_VertexFilePath = file_paths.vertexPath;
		return CreateFromFile(file_paths);
	}

	bool NewShader::CreateFromFile(const NewShaderFilePath& file_paths)
	{
		//m_ShaderFilePath.fragmentPath = file_paths.fragmentPath;Y
		m_FragFilePath = file_paths.fragmentPath;
		m_VertexFilePath = file_paths.vertexPath;
		std::string vString = ReadFile(file_paths.vertexPath);
		std::string fString = ReadFile(file_paths.fragmentPath);

		return CreateFromCode(vString.c_str(), fString.c_str());
	}


	std::string NewShader::ReadFile(const std::string& shader_file)
	{
		std::string content;
		std::ifstream fileStream(shader_file, std::ios::in);

		if (!fileStream.is_open())
		{
			printf("[Reading Shader]: Failed to read %s, file doesn't exist.", shader_file);
			return "";
		}

		std::string line = "";
		while (!fileStream.eof())
		{
			std::getline(fileStream, line);
			content.append(line + "\n");
		}

		fileStream.close();
		return content;
	}


	bool NewShader::CreateFromCode(const char* vCode, const char* fCode)
	{
		m_ProgramID = glCreateProgram();
		printf("Thge shader program Id is %d\n", m_ProgramID);

		GLuint vshader = CompileShader(GL_VERTEX_SHADER, vCode);
		GLuint fshader = CompileShader(GL_FRAGMENT_SHADER, fCode);

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


		return true;
	}


	unsigned int NewShader::CompileShader(GLenum type, const std::string& source)
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
			printf("[SHADER]: Couldn't create a %s, %s SOURCE: %s\n", LogShaderTypeName2(type), eLog, GetShaderFilePath(type));
			exit(-1);
		}

		return shaderid;
	}



	void NewShader::Bind() const
	{
		glUseProgram(m_ProgramID);
	}

	void NewShader::UnBind() const
	{
		glUseProgram(0);
	}


	int NewShader::GetUniformLocation(const char* name)
	{
		if (cacheUniformLocations.find(name) != cacheUniformLocations.end())
			return cacheUniformLocations[name];


		GLCall(int location = glGetUniformLocation(m_ProgramID, name));

		if (location == -1)
			printf("[SHADER UNIFORM (WARNING)]: uniform '%s' doesn't exist!!!!!\n", name);
		else
			cacheUniformLocations[name] = location;


		return location;
	}

	NewShader::~NewShader()
	{
		Clear();
		printf("I have been kick out \n");
	}

	void NewShader::Clear()
	{
		//TO-DO: not too sure but a program could accually been 0
		if (m_ProgramID != 0)
		{
			glDeleteProgram(m_ProgramID);
			m_ProgramID = 0;
		}
	}



	//TO-DO: need to rewrite works for now
	const char* NewShader::GetShaderFilePath(GLenum shader_type)
	{
		switch (shader_type)
		{
			case GL_VERTEX_SHADER: return m_VertexFilePath.c_str();
			case GL_FRAGMENT_SHADER: return m_FragFilePath.c_str();

			return "Not a registered shader";
		}
	}

	void NewShader::SetUniform1i(const char* name, int value)
	{
		GLCall(glUniform1i(GetUniformLocation(name), value));
	}

	void NewShader::SetUniform4f(const char* name, float v0, float v1, float v2, float v3)
	{
		GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
	}

	void NewShader::SetUniformMat4f(const char* name, const glm::mat4 matrix)
	{
		GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
	}


	void NewShader::SetUniformVec3(const char* name, const glm::vec3 vector)
	{
		GLCall(glUniform3f(GetUniformLocation(name), vector.x, vector.y, vector.z));
	}

	void NewShader::SetUniformVec4(const char* name, const glm::vec4 vector)
	{
		GLCall(glUniform4f(GetUniformLocation(name), vector.x, vector.y, vector.z, vector.w));
	}

	void NewShader::SetUniform1f(const char* name, float value)
	{
		GLCall(glUniform1f(GetUniformLocation(name), value));
	}

	void NewShader::SetUniformVec3f(const char* name, const float* value)
	{
		GLCall(glUniform3fv(GetUniformLocation(name), 1, value));
	}

