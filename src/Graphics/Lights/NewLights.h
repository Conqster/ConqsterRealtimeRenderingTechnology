#pragma once
#include <glm/glm.hpp>


enum LightType
{
	lightType_NONE,
	lightType_DIRECTIONAL,
	lightType_POINT,
	lightType_SPOT,
};



/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////LIGHTS////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

struct NewLight
{	
	float ambient[3]{0.2f, 0.2f, 0.2f};
	float diffuse[3]{0.35f, 0.35f, 0.35f};
	float specular[3]{0.75f, 0.75f, .75f};
	//TO-DO: for now use one colour, and just define
	//		define ambient,diffuse and specular as intensity

	glm::vec3 colour;

	float ambientIntensity;
	float diffuseIntensity;

	bool enable = false;

	LightType type = lightType_NONE;

	const char* LightTypeToString();

	NewLight(const glm::vec3& colour = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);

	virtual ~NewLight() = default;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////DIRECTIONAL LIGHT//////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

struct NewDirectionalLight : public NewLight
{
	glm::vec3 direction;

	NewDirectionalLight(const glm::vec3& dir = glm::vec3(-1.0f, 1.0f, 1.0f), const glm::vec3& col = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////POINT LIGHT//////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

struct NewPointLight : public NewLight
{
	glm::vec3 position;

	//So as to store all attenuation contiguously in memory
	//constant
	//linear
	//quardratic
	float attenuation[3];
	NewPointLight(const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& col = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////SPOT LIGHT///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

struct NewSpotLight : public NewPointLight
{
	glm::vec3 direction;

	//TO-DO: for now only use one float
	//		later convert to have
	//      innerCutoffAngle &
	//      outerCutoffAngle
	float innerCutoffAngle = 12.5f;
	float outerCutoffAngle = 17.5f;
	NewSpotLight(const glm::vec3& dir = glm::vec3(-0.2f, -1.0f, -0.3f), float cutoffAngle = 17.5f, 
		const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& col = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);
};


