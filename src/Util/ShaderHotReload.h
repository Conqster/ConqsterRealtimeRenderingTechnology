#pragma once
#include <memory>
#include <filesystem>

//forward declare
class Shader;

struct ShaderFileStamp
{
	Shader* shader;
	//vertex, frag, geometry
	std::filesystem::file_time_type timeStamp[3];
};


class ShaderHotReload
{
public: 
	ShaderHotReload()
	{
		printf("Shader Hot Reloader Init !!!!!!!!!!!!!!!!!!!!!!\n");
	}
	~ShaderHotReload()
	{
		printf("Shader Hot Reloader Closing !!!!!!!!!!!!!!!!!!!!!!\n");
		if (m_TrackingShader.shader)
			m_TrackingShader.shader = nullptr;
	}


	void TrackShader(Shader* shader);
	void Update();

private:
	ShaderFileStamp m_TrackingShader;
	unsigned int version = 0;


	/// <summary>
	/// Check shader file path for changes 
	/// type is the shader type index {vertex, frag, geomtry}
	/// </summary>
	bool CheckShaderFileChanges(ShaderFileStamp shader_fs, unsigned int curr_type_idx);

	void ReflectTimeStamp(ShaderFileStamp& shader_ts);

	time_t RetrieveTimeStamp(std::filesystem::file_time_type time_stamp);
};
