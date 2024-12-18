#pragma once
#include "Shader.h"
#include <fstream>

#include "RendererErrorAssertion.h"

	const char* LogShaderTypeName2(GLenum shaderType)
	{
		switch (shaderType)
		{
			case GL_VERTEX_SHADER: return "Vertex Shader";
			case GL_FRAGMENT_SHADER: return "Fragmant Shader";
			case GL_GEOMETRY_SHADER: return "Geometry Shader";

		}
		return "Not registed shader type";
	}


	bool Shader::Create(const std::string& name, const ShaderFilePath& file_paths)
	{
		m_Name = name;
		m_FragFilePath = file_paths.fragmentPath;
		m_VertexFilePath = file_paths.vertexPath;
		m_GeometryFilePath = file_paths.geometryPath;
		return CreateFromFile(file_paths);
	}

	bool Shader::Create(const std::string& name, const std::string& ver, const std::string& frag, const std::string& geo)
	{
		m_Name = name;
		m_VertexFilePath = ver;
		m_FragFilePath = frag;
		m_GeometryFilePath = geo;
		return CreateFromFile({ver, frag, geo});
	}

	bool Shader::CreateFromFile(const ShaderFilePath& file_paths)
	{
		//m_ShaderFilePath.fragmentPath = file_paths.fragmentPath;Y
		m_FragFilePath = file_paths.fragmentPath;
		m_VertexFilePath = file_paths.vertexPath;
		m_GeometryFilePath = file_paths.geometryPath;

		std::string vString = ReadFile(file_paths.vertexPath);
		std::string fString = ReadFile(file_paths.fragmentPath);

		//special shaders 
		std::string gString = "";
		if (!file_paths.geometryPath.empty())
			gString = ReadFile(file_paths.geometryPath);

		return CreateFromCode(m_ProgramID, vString.c_str(), fString.c_str(), gString);
	}

	bool Shader::SoftCreate(const std::string& name, const std::string& ver, const std::string& frag, const std::string& geo)
	{
		m_Name = name;
		m_VertexFilePath = ver;
		m_FragFilePath = frag;
		m_GeometryFilePath = geo;

		std::string vString = ReadFile(m_VertexFilePath);
		std::string fString = ReadFile(m_FragFilePath);

		//special shaders 
		std::string gString = "";
		if (!m_GeometryFilePath.empty())
			gString = ReadFile(m_GeometryFilePath);

		return CreateFromCode(m_ProgramID, vString.c_str(), fString.c_str(), gString, false);
	}




	std::string Shader::ReadFile(const std::string& shader_file)
	{
		std::string content;
		std::ifstream fileStream(shader_file, std::ios::in);

		if (!fileStream.is_open())
		{
			printf("[Reading Shader File (for %s)]: Failed to read %s, file doesn't exist.\n", m_Name.c_str(), shader_file.c_str());
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


	bool Shader::CreateFromCode(unsigned int& ID, const char* vCode, const char* fCode, const std::string& gCode, bool hard)
	{
		//m_ProgramID = glCreateProgram();
		ID = glCreateProgram();
		printf("The shader program '%s' ID: %d\n", m_Name.c_str(), ID);

		GLuint vshader = CompileShader(GL_VERTEX_SHADER, vCode, hard);
		GLuint fshader = CompileShader(GL_FRAGMENT_SHADER, fCode, hard);

		GLCall(glAttachShader(ID, vshader));
		GLCall(glAttachShader(ID, fshader));

		//Special shaders 
		GLuint gshader;
		if (!gCode.empty())
		{
			gshader = CompileShader(GL_GEOMETRY_SHADER, gCode);
			GLCall(glAttachShader(ID, gshader));
		}

		glLinkProgram(ID);
		glValidateProgram(ID);


		GLint result = 0;
		GLchar eLog[1024] = { 0 };
		glGetProgramiv(ID, GL_VALIDATE_STATUS, &result);

		if (!result)
		{
			glGetProgramInfoLog(ID, sizeof(eLog), NULL, eLog);
			printf("[ERROR VALIDATING PROGRAM (for %s)]: '%s'\n", m_Name.c_str(), eLog);


			//need to destroy shader if compiled 
			GLCall(glDeleteShader(vshader));
			GLCall(glDeleteShader(fshader));


			//speical shaders
			if (!gCode.empty())
			{
				GLCall(glDeleteShader(gshader));
			}

			if(hard)
				exit(-1);

			return false;
		}

		//glUseProgram(program);
		GLCall(glDeleteShader(vshader));
		GLCall(glDeleteShader(fshader));


		//speical shaders
		if (!gCode.empty())
		{
			GLCall(glDeleteShader(gshader));
		}



		return true;
	}


	unsigned int Shader::CompileShader(GLenum type, const std::string& source, bool hard)
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
			printf("[SHADER]: Couldn't create a %s, %sSOURCE: %s\n", LogShaderTypeName2(type), eLog, GetShaderFilePath(type));
			if(hard)
				exit(-1);
		}

		return shaderid;
	}



	void Shader::Bind() const
	{
		
		GLCall(glUseProgram(m_ProgramID));
		//glUseProgram(m_ProgramID);
	}

	void Shader::UnBind() const
	{
		//glUseProgram(0);
		GLCall(glUseProgram(0));
	}


	int Shader::GetUniformLocation(const char* name)
	{
		if (cacheUniformLocations.find(name) != cacheUniformLocations.end())
			return cacheUniformLocations[name];


		GLCall(int location = glGetUniformLocation(m_ProgramID, name));

		if (location == -1)
			printf("[SHADER UNIFORM (WARNING) program: %s]: uniform '%s' doesn't exist!!!!!\n", m_Name.c_str(), name);
		else
			cacheUniformLocations[name] = location;


		return location;
	}

	Shader::~Shader()
	{
		Clear();
		printf("[SHADER]: Closed/deleting \n");
	}

	void Shader::Clear()
	{
		//TO-DO: not too sure but a program could accually been 0
		if (m_ProgramID != 0)
		{
			glDeleteProgram(m_ProgramID);
			m_ProgramID = 0;
		}

		cacheUniformLocations.clear();
		cacheBindingBlocks.clear();
	}



	//TO-DO: need to rewrite works for now
	const char* Shader::GetShaderFilePath(GLenum shader_type)
	{
		switch (shader_type)
		{
			case GL_VERTEX_SHADER: return m_VertexFilePath.c_str();
			case GL_FRAGMENT_SHADER: return m_FragFilePath.c_str();
			case GL_GEOMETRY_SHADER: return m_GeometryFilePath.c_str();

		}
		return "Not a registered shader";
	}

	void Shader::SetUniformBlockIdx(const char* name, int blockBindingIdx)
	{
		const int idx = glGetUniformBlockIndex(m_ProgramID, name);
		GLCall(glUniformBlockBinding(m_ProgramID, idx, blockBindingIdx));

		ShaderBlockingIdx shader_block
		{
			name,
			blockBindingIdx
		};
		cacheBindingBlocks.push_back(shader_block);
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

	void Shader::SetUniformVec2(const std::string& name, const glm::vec2 vector)
	{
		GLCall(glUniform2f(GetUniformLocation(name.c_str()), vector.x, vector.y));
	}

	void Shader::SetUniformVec4(const char* name, const glm::vec4 vector)
	{
		GLCall(glUniform4f(GetUniformLocation(name), vector.x, vector.y, vector.z, vector.w));
	}

	void Shader::SetUniform1f(const char* name, float value)
	{
		GLCall(glUniform1f(GetUniformLocation(name), value));
	}

	void Shader::SetUniformVec3f(const char* name, const float* value)
	{
		GLCall(glUniform3fv(GetUniformLocation(name), 1, value));
	}

