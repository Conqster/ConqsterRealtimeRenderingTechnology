#pragma once

#include <yaml-cpp/yaml.h>
#include "glm/glm.hpp"



inline YAML::Emitter& operator << (YAML::Emitter& out, const glm::vec3& vec)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
	return out;
}


//directional Light 
inline YAML::Emitter& operator << (YAML::Emitter& out, const DirectionalLight dl)
{
	out << YAML::Key << "m_Direction" << dl.direction;
	out << YAML::Key << "m_Colour" << dl.colour;
	out << YAML::Key << "m_CastShadow" << YAML::Flow << dl.castShadow;
	out << YAML::Key << "m_Enable" << YAML::Flow << dl.enable;
	out << YAML::Key << "m_AmbinentIntensity" << YAML::Flow << dl.ambientIntensity;
	out << YAML::Key << "m_DiffuseIntensity" << YAML::Flow << dl.diffuseIntensity;
	out << YAML::Key << "m_SpecularIntensity" << YAML::Flow << dl.specularIntensity;
	return out;
}


//point light 
inline YAML::Emitter& operator << (YAML::Emitter& out, const PointLight pt)
{
	out << YAML::Key << "m_Position" << pt.position;
	out << YAML::Key << "m_Colour" << pt.colour;
	out << YAML::Key << "m_Attenuation" << glm::vec3(pt.attenuation[0], pt.attenuation[1], pt.attenuation[2]);
	out << YAML::Key << "m_CastShadow" << YAML::Flow << pt.castShadow;
	out << YAML::Key << "m_Enable" << YAML::Flow << pt.enable;
	out << YAML::Key << "m_AmbinentIntensity" << YAML::Flow << pt.ambientIntensity;
	out << YAML::Key << "m_DiffuseIntensity" << YAML::Flow << pt.diffuseIntensity;
	out << YAML::Key << "m_SpecularIntensity" << YAML::Flow << pt.specularIntensity;
	return out;
}


namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<double>();
			rhs.y = node[1].as<double>();
			rhs.z = node[2].as<double>();
			return true;
		}
	};

	template<>
	struct convert<DirectionalLight>
	{
		static Node encode(const DirectionalLight& rhs)
		{
			Node node;
			node["m_Direction"] = rhs.direction;
			node["m_Colour"] = rhs.colour;
			node["m_CastShadow"] = rhs.castShadow;
			node["m_Enable"] = rhs.enable;
			node["m_AmbinentIntensity"] = rhs.ambientIntensity;
			node["m_DiffuseIntensity"] = rhs.diffuseIntensity;
			node["m_SpecularIntensity"] = rhs.specularIntensity;
			return node;
		}

		static bool decode(const Node& node, DirectionalLight& rhs)
		{
			if (!node.IsMap() || node.size() != 7)
			{
				printf("FAILED TO DECODE DIRECTIONAL LIGHT\n");
				return false;
			}

			//printf("SUCCESSFULLY DECODE DIRECTIONAL LIGHT\n");
			rhs.direction = node["m_Direction"].as<glm::vec3>();
			rhs.colour = node["m_Colour"].as<glm::vec3>();
			rhs.castShadow = node["m_CastShadow"].as<bool>();
			rhs.enable = node["m_Enable"].as<bool>();
			rhs.ambientIntensity = node["m_AmbinentIntensity"].as<float>();
			rhs.diffuseIntensity = node["m_DiffuseIntensity"].as<float>();
			rhs.specularIntensity = node["m_SpecularIntensity"].as<float>();
			return true;
		}
	};


	template<>
	struct convert<PointLight>
	{
		static Node encode(const PointLight& rhs)
		{
			Node node;
			node["m_Position"] = rhs.position;
			node["m_Colour"] = rhs.colour;
			//node["m_Attenuation"] = rhs.attenuation;
			node["m_CastShadow"] = rhs.castShadow;
			node["m_Enable"] = rhs.enable;
			node["m_AmbinentIntensity"] = rhs.ambientIntensity;
			node["m_DiffuseIntensity"] = rhs.diffuseIntensity;
			node["m_SpecularIntensity"] = rhs.specularIntensity;
			return node;
		}

		static bool decode(const Node& node, PointLight& rhs)
		{
			if (!node.IsMap() || node.size() != 8)
			{
				printf("FAILED TO DECODE POINT LIGHT\n");
				return false;
			}

			//printf("SUCCESSFULLY DECODE POINT LIGHT\n");
			rhs.position = node["m_Position"].as<glm::vec3>();
			rhs.colour = node["m_Colour"].as<glm::vec3>();
			glm::vec3 att_vec = node["m_Attenuation"].as<glm::vec3>();
			rhs.attenuation[0] = att_vec.x;
			rhs.attenuation[1] = att_vec.y;
			rhs.attenuation[2] = att_vec.z;
			rhs.castShadow = node["m_CastShadow"].as<bool>();
			rhs.enable = node["m_Enable"].as<bool>();
			rhs.ambientIntensity = node["m_AmbinentIntensity"].as<float>();
			rhs.diffuseIntensity = node["m_DiffuseIntensity"].as<float>();
			rhs.specularIntensity = node["m_SpecularIntensity"].as<float>();
			return true;
		}
	};
}
