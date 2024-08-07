#pragma once
#include "NewLights.h"
#include <iostream>


	NewLight::NewLight(const glm::vec3& colour, float amb_inten, float diff_inten)
		: colour(colour), ambientIntensity(amb_inten), diffuseIntensity(diff_inten)
	{
	}

	const char* NewLight::LightTypeToString()
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


	/////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////DIRECTIONAL LIGHT//////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////

	NewDirectionalLight::NewDirectionalLight(const glm::vec3& dir, const glm::vec3& col, float amb_inten, float diff_inten)
						: direction(dir), NewLight(col, amb_inten, diff_inten)
	{
		type = lightType_DIRECTIONAL;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////POINT LIGHT//////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////

	NewPointLight::NewPointLight(const glm::vec3& pos, const glm::vec3& col, float amb_inten, float diff_inten)
		: NewLight(col, amb_inten, diff_inten), position(pos), attenuation{1.0f, 0.2f, 0.03f}
	{
		type = lightType_POINT;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////SPOT LIGHT///////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////


	NewSpotLight::NewSpotLight(const glm::vec3& dir, float cutoff_angle, const glm::vec3& pos, const glm::vec3& col, float amb_inten, float diff_inten)
		:direction(dir), outerCutoffAngle(cutoff_angle), NewPointLight(pos, col, amb_inten, diff_inten)
	{
		//innerCutoffAngle = (innerCutoffAngle < 0) ? 0 : innerCutoffAngle;
		type = lightType_SPOT;
	}
