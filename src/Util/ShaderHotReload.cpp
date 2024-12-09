#include "ShaderHotReload.h"
#include "Renderer/Shader.h"


#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>


void ShaderHotReload::TrackShader(Shader* shader)
{
	printf("Adding shader %s for hot reload tracking\n", (shader->GetName()).c_str());

	//record vertex file time stamp
	m_TrackingShader = { shader };
	ReflectTimeStamp(m_TrackingShader);
	//auto time_stamp = std::filesystem::last_write_time(shader->GetShaderFilePath(GL_VERTEX_SHADER));

	//m_TrackingShader = { shader, time_stamp };
}

void ShaderHotReload::Update()
{

	if (m_TrackingShader.shader)
	{
		bool changed = CheckShaderFileChanges(m_TrackingShader, 0);
		changed |= CheckShaderFileChanges(m_TrackingShader, 1);
		changed |= CheckShaderFileChanges(m_TrackingShader, 2);

		if (changed)
		{
			printf("Hot load a changed shader !!!!!!!!!!!!!\n");
			Shader* new_shader = new Shader();
			bool success = new_shader->SoftCreate(("model_forward_shading v" + std::to_string(version++)),
				m_TrackingShader.shader->GetShaderFilePath(GL_VERTEX_SHADER),
				m_TrackingShader.shader->GetShaderFilePath(GL_FRAGMENT_SHADER),
				m_TrackingShader.shader->GetShaderFilePath(GL_GEOMETRY_SHADER));



			if (success)
			{
				for (auto& block : m_TrackingShader.shader->GetBindingBlocks())
					new_shader->SetUniformBlockIdx((block.name).c_str(), block.idx);
				m_TrackingShader.shader->Clear();
				*m_TrackingShader.shader = *new_shader;
				//*m_TrackingShader.shader = Shader(new_shader);
			}

			ReflectTimeStamp(m_TrackingShader);
			//m_TrackingShader.shader->Reload();
			//m_TrackingShader.timeStamp = ftime;
		}

		//auto cftime = RetrieveTimeStamp(ftime);
		//std::cout << "file last modification is " << std::put_time(std::localtime(&cftime), "%F %T") << '\n';


		//printf("Time stamp is %s.\n", (m_TrackingShader.timeStamp == ftime) ? "the same" : "not the same");
	}
	else
	{
		printf("No Shader to Hot Reload!!!!!!!!\n");
	}
}

bool ShaderHotReload::CheckShaderFileChanges(ShaderFileStamp shader_fs, unsigned int curr_type_idx)
{
	//definatly need to fix this
	GLenum shader_type = GL_VERTEX_SHADER;
	if (curr_type_idx == 1)
		shader_type = GL_FRAGMENT_SHADER;
	else if (curr_type_idx == 2)
		shader_type = GL_GEOMETRY_SHADER;
	
	std::string file_path = shader_fs.shader->GetShaderFilePath(shader_type);

	//"" is use for empty shader, 
	//for instead a shader might have both vert & frag but no geometry shader 
	if (file_path == "")
		return false;

	if (!std::filesystem::exists(file_path))
	{
		printf("[CheckShaderFileChanges]: File does not exist !!! \n");
		return false;
	}
	auto ftime = std::filesystem::last_write_time(file_path);

	return (ftime != shader_fs.timeStamp[curr_type_idx]);
}

time_t ShaderHotReload::RetrieveTimeStamp(std::filesystem::file_time_type time_stamp)
{
	auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>
		(time_stamp - std::filesystem::file_time_type::clock::now() +
			std::chrono::system_clock::now());

	return std::chrono::system_clock::to_time_t(sctp);
}

void ShaderHotReload::ReflectTimeStamp(ShaderFileStamp& shader_ts)
{
	auto& shader = shader_ts.shader;
	if(std::filesystem::exists(shader->GetShaderFilePath(GL_VERTEX_SHADER)))
		shader_ts.timeStamp[0] = std::filesystem::last_write_time(shader->GetShaderFilePath(GL_VERTEX_SHADER));
	if(std::filesystem::exists(shader->GetShaderFilePath(GL_FRAGMENT_SHADER)))
		shader_ts.timeStamp[1] = std::filesystem::last_write_time(shader->GetShaderFilePath(GL_FRAGMENT_SHADER));
	if(std::filesystem::exists(shader->GetShaderFilePath(GL_GEOMETRY_SHADER)))
		shader_ts.timeStamp[2] = std::filesystem::last_write_time(shader->GetShaderFilePath(GL_GEOMETRY_SHADER));
}
