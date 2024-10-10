#pragma once
#include "Lights.h"
#include <iostream>

#include "../ObjectBuffer/UniformBuffer.h"

	Light::Light(const glm::vec3& colour, float amb_inten, float diff_inten)
		: colour(colour), ambientIntensity(amb_inten), diffuseIntensity(diff_inten)
	{
	}

	const char* Light::LightTypeToString()
	{
		switch (type)
		{
		case lightType_NONE: return "NONE";
		case lightType_DIRECTIONAL: return "Directional Light";
		case lightType_POINT: return "Point Light";
		case lightType_SPOT: return "Spot Light";
		}
		return "[Error]: Type not configured or null";
	}


	
	//------------------------------------DIRECTIONAL LIGHT-----------------------------------------/
	

	DirectionalLight::DirectionalLight(const glm::vec3& dir, const glm::vec3& col, float amb_inten, float diff_inten)
						: direction(dir), Light(col, amb_inten, diff_inten)
	{
		type = lightType_DIRECTIONAL;
	}

	void DirectionalLight::UpdateUniformBufferData(UniformBuffer& ubo, long long int& offset_pointer)
	{
		long long int vec3_size = sizeof(glm::vec3);
		long long int float_size = sizeof(float);


		//colour
		ubo.SetSubDataByID(&colour[0], vec3_size, offset_pointer);
		offset_pointer += vec3_size;
		//enable
		ubo.SetSubDataByID(&enable, sizeof(int), offset_pointer);
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
		type = lightType_POINT;
	}

	void PointLight::UpdateUniformBufferData(UniformBuffer& ubo, long long int& offset_pointer)
	{
		long long int vec3_size = sizeof(glm::vec3);
		long long int float_size = sizeof(float);

		//colour
		ubo.SetSubDataByID(&colour[0], vec3_size, offset_pointer);
		offset_pointer += vec3_size;
		//enable
		ubo.SetSubDataByID(&enable, sizeof(int), offset_pointer);
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
		type = lightType_SPOT;
	}
