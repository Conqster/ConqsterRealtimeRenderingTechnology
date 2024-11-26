#include "SceneSerialiser.h"
#include <fstream>

#include "Renderer/Shader.h"

bool SceneSerialiser::LoadScene(const std::string& file, SceneData& scene_data)
{
	int success = 0;
	YAML::Node root_node = YAML::LoadFile(file);


	//Scene utilities
	if (root_node["m_HasLight"])
		scene_data.m_HasLight = root_node["m_HasLight"].as<bool>();

	if (root_node["m_HasShadow"])
		scene_data.m_HasShadow = root_node["m_HasShadow"].as<bool>();

	if (root_node["m_HasEnvironmentMap"])
		scene_data.m_HasEnvironmentMap = root_node["m_HasEnvironmentMap"].as<bool>();

	//Retrive camera
	if (root_node["Main Camera"])
		success += RetrieveCameraData(root_node["Main Camera"], scene_data.m_Camera);

	//Retrieve render config 
	if (root_node["Rendering Config"])
		success += RetrieveRenderConfig(root_node["Rendering Config"], scene_data.m_RenderingConfig);

	//Retrieve environment setting
	if (root_node["m_Environment Setting"])
		success += RetrieveEnvironmentData(root_node["m_Environment Setting"], scene_data.m_SceneEnvi);

	//Retrieve light 
	 success += RetrieveLights(root_node, scene_data.m_Light);

	//Retrieve shadow setting
	 if(root_node["Shadow Setting"])
		success += RetrieveShadowSetting(root_node["Shadow Setting"], scene_data.m_Shadow);

	return (success >= 4);
}

bool SceneSerialiser::RetrieveLights(const YAML::Node data, SceneLightData& scene_light)
{
	bool failed = false;
	if (data["Dir light"])
		scene_light.dir_Light = data["Dir light"].as<DirectionalLight>();


	if (data["Point Lights"]["m_Counts"])
		scene_light.m_PtLightCount = data["Point Lights"]["m_Counts"].as<int>();

	scene_light.ptLights = new PointLight[scene_light.m_PtLightCount];
	for (int i = 0; i < scene_light.m_PtLightCount; i++)
	{
		if (data["Point Lights"][i])
			scene_light.ptLights[i] = data["Point Lights"][i].as<PointLight>();
	}

	return !failed;
}

bool SceneSerialiser::RetrieveRenderConfig(const YAML::Node data, SceneRenderingConfig& render_config)
{
	if (!data.IsMap() || data.size() != 1)
	{
		printf("FAILED TO RETRIEVE RENDER CONFIG\n");
		return false;
	}

	if (data["m_ForwardShader"])
		RetrieveShader(data["m_ForwardShader"], render_config.m_ForwardShader);
	else
		printf("[NODE ERROR]: m_Forward Shader node does not exist!!!!\n");

	return true;
}

bool SceneSerialiser::RetrieveEnvironmentData(const YAML::Node data, SceneEnvironment& envir_data)
{
	if (!data.IsMap() || data.size() != 5)
	{
		printf("FAILED TO RETRIEVE ENVIRONMENT DATA\n");
		return false;
	}

	bool failed = false;
	envir_data.m_EnableSkybox = data["m_EnableSkybox"].as<bool>();
	envir_data.m_SkyboxInfluencity = data["m_SkyboxInfluencity"].as<float>();
	envir_data.m_SkyboxReflectivity = data["m_SkyboxReflectivity"].as<float>();

	//skybox texture
	auto& tex_data = data["m_Skybox Texture"];
	if (tex_data && tex_data.IsMap() && tex_data.size() == 6)
	{
		envir_data.m_SkyboxTex.tex_rt = tex_data["right"].as<std::string>();
		envir_data.m_SkyboxTex.tex_lt = tex_data["left"].as<std::string>();
		envir_data.m_SkyboxTex.tex_tp = tex_data["top"].as<std::string>();
		envir_data.m_SkyboxTex.tex_bm = tex_data["bottom"].as<std::string>();
		envir_data.m_SkyboxTex.tex_ft = tex_data["front"].as<std::string>();
		envir_data.m_SkyboxTex.tex_bk = tex_data["back"].as<std::string>();
	}
	else
		failed |= true, printf("FAILED TO LOAD/RETRIEVE SKYBOX TEXTURE\n");

	//skybox shader 
	if (data["m_Skybox Shader"])
		failed |= RetrieveShader(data["m_Skybox Shader"], envir_data.m_SkyboxShader);

	return !failed;
}

bool SceneSerialiser::RetrieveCameraData(const YAML::Node cam_node, SceneCameraData& scene_camera)
{
	if (!cam_node.IsMap() || cam_node.size() != 8)
	{
		printf("FAILED TO RETRIEVE CAMERA DATA\n");
		return false;
	}

	scene_camera.m_Position = cam_node["m_Position"].as<glm::vec3>();
	scene_camera.m_Yaw = cam_node["m_Yaw"].as<float>();
	scene_camera.m_Pitch = cam_node["m_Pitch"].as<float>();
	scene_camera.m_Fov = cam_node["m_Fov"].as<float>();
	scene_camera.m_Near = cam_node["m_Near"].as<float>();
	scene_camera.m_Far = cam_node["m_Far"].as<float>();
	scene_camera.m_MoveSpeed = cam_node["m_MoveSpeed"].as<float>();
	scene_camera.m_RotSpeed = cam_node["m_RotSpeed"].as<float>();

	return true;
}

bool SceneSerialiser::RetrieveShadowSetting(const YAML::Node sh_data, SceneShadowData& shadow_data)
{
	if (!sh_data.IsMap() || sh_data.size() != 3)
	{
		printf("FAILED TO RETRIEVE SHADOW SETTINGS\n");
		return false;
	}

	bool failed = false;
	YAML::Node dir = sh_data["Directional Shadow"];
	if (dir && dir.IsMap() && dir.size() == 6)
	{
		shadow_data.m_DirDepthResQuality = dir["m_ResolutionQuality"].as<int>();
		shadow_data.m_DirOrthoSize = dir["m_OrthoSize"].as<float>();
		shadow_data.m_DirSamplePos = dir["m_SamplePos"].as<glm::vec3>();
		shadow_data.m_DirSampleOffset = dir["m_SampleOffset"].as<float>();
		shadow_data.m_DirZFar = dir["m_zFar"].as<float>();
		shadow_data.m_DirZNear = dir["m_zNear"].as<float>();
	}
	else
		failed |= true, printf("FAILED TO LOAD/RETRIEVE DIRECTIONAL SHADOW\n");

	if (sh_data["OmniDirectional Shadow"])
	{
		shadow_data.m_PtDepthResQuality = sh_data["OmniDirectional Shadow"]["m_ResolutionQuality"].as<int>();
		shadow_data.m_PtZNear = sh_data["OmniDirectional Shadow"]["m_zNear"].as<float>();
	}
	else
		failed |= true, printf("FAILED TO LOAD/RETRIEVE OMNIDIRECTIONAL SHADOW\n");

	YAML::Node util = sh_data["Utilities"];
	if (util && util.IsMap() && util.size() == 3)
	{
		shadow_data.m_depthVerShader = util["m_VertexShader"].as<std::string>();
		shadow_data.m_depthFragShader = util["m_FragmentShader"].as<std::string>();
		shadow_data.m_depthGeoShader = util["m_GeometryShader"].as<std::string>();
	}
	else
		failed |= true, printf("FAILED TO LOAD/RETRIEVE SHADOW UTILITIES\n");

	return !failed;
}

bool SceneSerialiser::RetrieveShader(const YAML::Node data, ShaderData& shader_data)
{
	if (!data.IsMap() || data.size() != 4)
	{
		printf("FAILED TO RETRIEVE SHADER\n");
		return false;
	}

	shader_data.m_Name = data["name"].as<std::string>();
	shader_data.m_Vertex = data["vertex"].as<std::string>();
	shader_data.m_Fragment = data["fragment"].as<std::string>();
	shader_data.m_Geometry = data["geometry"].as<std::string>();
	return true;
}

void SceneSerialiser::SerialiseScene(const std::string& file, SceneData scene_data)
{
	YAML::Emitter out;
	out << YAML::BeginMap;

	//Scene utilities
		out << YAML::Key << "m_Name" << YAML::Value << scene_data.m_Name;
		out << YAML::Key << "m_HasLights" << YAML::Value << scene_data.m_HasLight;
		out << YAML::Key << "m_HasShadow" << YAML::Value << scene_data.m_HasShadow;
		out << YAML::Key << "m_HasEnvironmentMap" << YAML::Value << scene_data.m_HasEnvironmentMap;

		//Camera Data
		SerialiseCameraData(out, scene_data.m_Camera);

		//Rendering config
		SerialiseRenderingConfig(out, scene_data.m_RenderingConfig);

		//environment setting (Skybox's texture, shader etc) 
		SerialiseEnvironmentData(out, scene_data.m_SceneEnvi);

		//Shadow settings (Dir & Omni, Res Quality, Sample location, z)
		SerialiseShadowSetting(out, scene_data.m_Shadow);

		//dir & point light 
		SerialiseLightData(out, scene_data.m_Light);

	out << YAML::EndMap;

	std::ofstream fout(file);
	fout << out.c_str();
}

//void SceneSerialiser::SerialiseShader(const std::string& file, Shader shader)
//{
//	YAML::Emitter out;
//	out << YAML::BeginMap;
//		out << YAML::Key << "Name" << YAML::Value << shader.m_Name;
//		out << YAML::Key << "Paths";
//		out << YAML::BeginMap;
//			out << YAML::Key << "vertex" << YAML::Value << shader.m_VertexFilePath;
//			out << YAML::Key << "fragment" << YAML::Value << shader.m_FragFilePath;
//			out << YAML::Key << "geometry" << YAML::Value << shader.m_GeometryFilePath;
//		out << YAML::EndMap;
//	out << YAML::EndMap;
//
//	std::ofstream fout(file);
//	fout << out.c_str();
//}

void SceneSerialiser::SerialiseLightData(YAML::Emitter& out, SceneLightData& light_data)
{
	//direction light 
	out << YAML::Key << "Dir light";
	out << YAML::BeginMap;
		out << light_data.dir_Light;
	out << YAML::EndMap;

	//point light 
	out << YAML::Key << "Point Lights";
	out << YAML::BeginMap;
		out << YAML::Key << "m_Counts" << YAML::Value << light_data.m_PtLightCount;
		for (int i = 0; i < light_data.m_PtLightCount; i++)
		{
			out << YAML::Key << i;
			out << YAML::BeginMap;
				out << light_data.ptLights[i];
			out << YAML::EndMap;
		}
	out << YAML::EndMap;
}

void SceneSerialiser::SerialiseRenderingConfig(YAML::Emitter& out, SceneRenderingConfig& rendering_config)
{
	out << YAML::Key << "Rendering Config";
	out << YAML::BeginMap;
		out << YAML::Key << "m_ForwardShader";
		out << YAML::BeginMap;
			out << YAML::Key << "name" << YAML::Key << rendering_config.m_ForwardShader.m_Name;
			out << YAML::Key << "vertex" << YAML::Key << rendering_config.m_ForwardShader.m_Vertex;
			out << YAML::Key << "fragment" << YAML::Key << rendering_config.m_ForwardShader.m_Fragment;
			out << YAML::Key << "geometry" << YAML::Key << rendering_config.m_ForwardShader.m_Geometry;
		out << YAML::EndMap;
	out << YAML::EndMap;
}

void SceneSerialiser::SerialiseEnvironmentData(YAML::Emitter& out, SceneEnvironment& envir_data)
{
	out << YAML::Key << "m_Environment Setting";
	out << YAML::BeginMap;
		out << YAML::Key << "m_EnableSkybox" << YAML::Key << envir_data.m_EnableSkybox;
		out << YAML::Key << "m_SkyboxInfluencity" << YAML::Key << envir_data.m_SkyboxInfluencity;
		out << YAML::Key << "m_SkyboxReflectivity" << YAML::Key << envir_data.m_SkyboxReflectivity;
		out << YAML::Key << "m_Skybox Texture";
		out << YAML::BeginMap;
			out << YAML::Key << "right" << YAML::Key << envir_data.m_SkyboxTex.tex_rt;
			out << YAML::Key << "left" << YAML::Key << envir_data.m_SkyboxTex.tex_lt;
			out << YAML::Key << "top" << YAML::Key << envir_data.m_SkyboxTex.tex_tp;
			out << YAML::Key << "bottom" << YAML::Key << envir_data.m_SkyboxTex.tex_bm;
			out << YAML::Key << "front" << YAML::Key << envir_data.m_SkyboxTex.tex_ft;
			out << YAML::Key << "back" << YAML::Key << envir_data.m_SkyboxTex.tex_bk;
		out << YAML::EndMap;
		out << YAML::Key << "m_Skybox Shader";
		SerialiseShader(out, envir_data.m_SkyboxShader);
	out << YAML::EndMap;
}

void SceneSerialiser::SerialiseCameraData(YAML::Emitter& out, SceneCameraData& camera_data)
{
	out << YAML::Key << "Main Camera";
	out << YAML::BeginMap;
		out << YAML::Key << "m_Position" << camera_data.m_Position;
		out << YAML::Key << "m_Yaw" << camera_data.m_Yaw;
		out << YAML::Key << "m_Pitch" << camera_data.m_Pitch;
		out << YAML::Key << "m_Fov" << camera_data.m_Fov;
		out << YAML::Key << "m_Near" << camera_data.m_Near;
		out << YAML::Key << "m_Far" << camera_data.m_Far;
		out << YAML::Key << "m_MoveSpeed" << camera_data.m_MoveSpeed;
		out << YAML::Key << "m_RotSpeed" << camera_data.m_RotSpeed;
	out << YAML::EndMap;
}

void SceneSerialiser::SerialiseShadowSetting(YAML::Emitter& out, SceneShadowData& shadow_data)
{
	out << YAML::Key << "Shadow Setting";
	out << YAML::BeginMap;
		out << YAML::Key << "Directional Shadow";
		out << YAML::BeginMap;
			out << YAML::Key << "m_ResolutionQuality" << shadow_data.m_DirDepthResQuality;
			out << YAML::Key << "m_OrthoSize" << shadow_data.m_DirOrthoSize;
			out << YAML::Key << "m_SamplePos" << shadow_data.m_DirSamplePos;
			out << YAML::Key << "m_zFar" << shadow_data.m_DirZFar;
			out << YAML::Key << "m_zNear" << shadow_data.m_DirZNear;
			out << YAML::Key << "m_SampleOffset" << shadow_data.m_DirSampleOffset;
		out << YAML::EndMap;		

		out << YAML::Key << "OmniDirectional Shadow";
		out << YAML::BeginMap;
			out << YAML::Key << "m_ResolutionQuality" << shadow_data.m_PtDepthResQuality;
			out << YAML::Key << "m_zFar" << shadow_data.m_PtZFar;
			out << YAML::Key << "m_zNear" << shadow_data.m_PtZNear;
		out << YAML::EndMap;

		out << YAML::Key << "Utilities";
		out << YAML::BeginMap;
			out << YAML::Key << "m_VertexShader" << shadow_data.m_depthVerShader;
			out << YAML::Key << "m_FragmentShader" << shadow_data.m_depthFragShader;
			out << YAML::Key << "m_GeometryShader" << shadow_data.m_depthGeoShader;
		out << YAML::EndMap;
	out << YAML::EndMap;
}

void SceneSerialiser::SerialiseShader(YAML::Emitter& out, ShaderData shader_data)
{
	out << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Key << shader_data.m_Name;
	out << YAML::Key << "vertex" << YAML::Key << shader_data.m_Vertex;
	out << YAML::Key << "fragment" << YAML::Key << shader_data.m_Fragment;
	out << YAML::Key << "geometry" << YAML::Key << shader_data.m_Geometry;
	out << YAML::EndMap;
}
