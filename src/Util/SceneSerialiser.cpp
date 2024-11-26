#include "SceneSerialiser.h"
#include <fstream>

#include "Renderer/Shader.h"

bool SceneSerialiser::LoadScene(const std::string& file, SceneData& scene_data)
{
	int success = 0;
	YAML::Node root_node = YAML::LoadFile(file);

	//Retrive camera
	if (root_node["Main Camera"])
		success += RetrieveCameraData(root_node["Main Camera"], scene_data.m_Camera);

	//Retrieve light 
	 success += RetrieveLights(root_node, scene_data.m_Light);

	//Retrieve shadow setting
	 if(root_node["Shadow Setting"])
		success += RetrieveShadowSetting(root_node["Shadow Setting"], scene_data.m_Shadow);

	return (success >= 3);
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

bool SceneSerialiser::RetrieveCameraData(const YAML::Node cam_node, SceneCameraData& scene_camera)
{
	if (!cam_node.IsMap() || cam_node.size() != 6)
	{
		printf("FAILED TO RETRIEVE CAMERA DATA\n");
		return false;
	}

	scene_camera.m_Position = cam_node["m_Position"].as<glm::vec3>();
	scene_camera.m_Up = cam_node["m_Up"].as<glm::vec3>();
	scene_camera.m_Yaw = cam_node["m_Yaw"].as<float>();
	scene_camera.m_Pitch = cam_node["m_Pitch"].as<float>();
	scene_camera.m_MoveSpeed = cam_node["m_MoveSpeed"].as<float>();
	scene_camera.m_RotSpeed = cam_node["m_RotSpeed"].as<float>();

	return true;
}

bool SceneSerialiser::RetrieveShadowSetting(const YAML::Node sh_data, SceneShadowData& shadow_data)
{
	if (!sh_data.IsMap() || sh_data.size() != 3)
	{
		printf("FAILED TO DECODE SHADOW SETTINGS\n");
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

void SceneSerialiser::SerialiseScene(const std::string& file, SceneData scene_data)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
		out << YAML::Key << "m_Name" << YAML::Value << scene_data.m_Name;
		out << YAML::Key << "m_HasLights" << YAML::Value << scene_data.m_HasLight;

		//Camera Data
		SerialiseCameraData(out, scene_data.m_Camera);

		//Shadow settings (Dir & Omni, Res Quality, Sample location, z)
		SerialiseShadowSetting(out, scene_data.m_Shadow);

		//dir & point light 
		SerialiseLightData(out, scene_data.m_Light);

	out << YAML::EndMap;

	std::ofstream fout(file);
	fout << out.c_str();
}

void SceneSerialiser::SerialiseShader(const std::string& file, Shader shader)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << shader.m_Name;
		out << YAML::Key << "Paths";
		out << YAML::BeginMap;
			out << YAML::Key << "vertex" << YAML::Value << shader.m_VertexFilePath;
			out << YAML::Key << "fragment" << YAML::Value << shader.m_FragFilePath;
			out << YAML::Key << "geometry" << YAML::Value << shader.m_GeometryFilePath;
		out << YAML::EndMap;
	out << YAML::EndMap;

	std::ofstream fout(file);
	fout << out.c_str();
}

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

void SceneSerialiser::SerialiseCameraData(YAML::Emitter& out, SceneCameraData& camera_data)
{
	out << YAML::Key << "Main Camera";
	out << YAML::BeginMap;
		out << YAML::Key << "m_Position" << camera_data.m_Position;
		out << YAML::Key << "m_Up" << camera_data.m_Up;
		out << YAML::Key << "m_Yaw" << camera_data.m_Yaw;
		out << YAML::Key << "m_Pitch" << camera_data.m_Pitch;
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
