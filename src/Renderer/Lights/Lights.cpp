#pragma once
#include "Lights.h"
#include <iostream>

#include "../ObjectBuffer/UniformBuffer.h"

	Light::Light(const glm::vec3& colour, float amb_inten, float diff_inten, float spec)
			    : colour(colour), ambientIntensity(amb_inten), 
				  diffuseIntensity(diff_inten), specularIntensity(spec)
	{
	}

	const char* Light::LightTypeToString()
	{
		switch (type)
		{
		case LightType::lightType_NONE: return "NONE";
		case LightType::lightType_DIRECTIONAL: return "Directional Light";
		case LightType::lightType_POINT: return "Point Light";
		case LightType::lightType_SPOT: return "Spot Light";
		}
		return "[Error]: Type not configured or null";
	}


	
	//------------------------------------DIRECTIONAL LIGHT-----------------------------------------/
	

	DirectionalLight::DirectionalLight(const glm::vec3& dir, const glm::vec3& col, float amb_inten, float diff_inten, float spec)
						: direction(dir), Light(col, amb_inten, diff_inten, spec)
	{
		type = LightType::lightType_DIRECTIONAL;
	}

	void DirectionalLight::UpdateUniformBufferData(UniformBuffer& ubo, unsigned int& offset_pointer)
	{
		unsigned int vec3_size = sizeof(glm::vec3);
		unsigned int float_size = sizeof(float);


		//colour
		ubo.SetSubDataByID(&colour[0], vec3_size, offset_pointer);
		offset_pointer += vec3_size;
		//enable
		ubo.SetSubDataByID(&enable, sizeof(bool), offset_pointer); //needs to 12 byte
		offset_pointer += sizeof(int);
		//direction
		ubo.SetSubDataByID(&direction[0], vec3_size, offset_pointer);
		offset_pointer += vec3_size;
		//ambient intensity
		ubo.SetSubDataByID(&ambientIntensity, float_size, offset_pointer);
		offset_pointer += float_size;
		//diffuse
		ubo.SetSubDataByID(&diffuseIntensity, float_size, offset_pointer);
		offset_pointer += float_size;
		//specular
		ubo.SetSubDataByID(&specularIntensity, float_size, offset_pointer);
		offset_pointer += float_size;

		//offset alignment padding
		offset_pointer += sizeof(glm::vec2);
	}


	
	//--------------------------------------POINT LIGHT--------------------------------------------/
	

	PointLight::PointLight(const glm::vec3& pos, const glm::vec3& col, float amb_inten, float diff_inten)
		: Light(col, amb_inten, diff_inten), position(pos), attenuation{1.0f, 0.2f, 0.03f}
	{
		type = LightType::lightType_POINT;
	}

	void PointLight::UpdateUniformBufferData(UniformBuffer& ubo, unsigned int& offset_pointer)
	{
		unsigned int vec3_size = sizeof(glm::vec3);
		unsigned int float_size = sizeof(float);

		//colour
		ubo.SetSubDataByID(&colour[0], vec3_size, offset_pointer);
		offset_pointer += vec3_size;
		//enable
		//gpu renderdoc as 3452764160 (false) -- 3452764161 (true) 
		//try fixing with boolean 0 - 1
		ubo.SetSubDataByID(&enable, sizeof(bool), offset_pointer); //first needs to be 60 bytes
		offset_pointer += sizeof(int);
		//position
		ubo.SetSubDataByID(&position[0], vec3_size, offset_pointer);
		offset_pointer += vec3_size;
		//ambient intensity
		ubo.SetSubDataByID(&ambientIntensity, float_size, offset_pointer);
		offset_pointer += float_size;
		//attenuation
		ubo.SetSubDataByID(&attenuation[0], vec3_size, offset_pointer);
		offset_pointer += vec3_size;
		//diffuse
		ubo.SetSubDataByID(&diffuseIntensity, float_size, offset_pointer);
		offset_pointer += float_size;
		//specular
		ubo.SetSubDataByID(&specularIntensity, float_size, offset_pointer);
		offset_pointer += float_size;
		//specular
		ubo.SetSubDataByID(&shadow_far, float_size, offset_pointer);
		offset_pointer += float_size;

		//alignement padding
		offset_pointer += sizeof(glm::vec2);

	}

	
	//--------------------------------------SPOT LIGHT----------------------------------------------/
	


	SpotLight::SpotLight(const glm::vec3& dir, float cutoff_angle, const glm::vec3& pos, const glm::vec3& col, float amb_inten, float diff_inten)
		:direction(dir), outerCutoffAngle(cutoff_angle), PointLight(pos, col, amb_inten, diff_inten)
	{
		//innerCutoffAngle = (innerCutoffAngle < 0) ? 0 : innerCutoffAngle;
		type = LightType::lightType_SPOT;
	}
