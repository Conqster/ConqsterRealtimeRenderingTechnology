#pragma once
#include <glm/glm.hpp>


enum class LightType
{
	lightType_NONE,
	lightType_DIRECTIONAL,
	lightType_POINT,
	lightType_SPOT,
};



/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////LIGHTS////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

struct Light
{	
	float ambient[3]{0.2f, 0.2f, 0.2f};
	float diffuse[3]{0.35f, 0.35f, 0.35f};
	float specular[3]{0.75f, 0.75f, .75f};
	//TO-DO: for now use one colour, and just define
	//		define ambient,diffuse and specular as intensity

	glm::vec3 colour;

	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;

	bool enable = false;
	bool castShadow = false;

	LightType type = LightType::lightType_NONE;

	const char* LightTypeToString();

	Light(const glm::vec3& colour = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);
	virtual void UpdateUniformBufferData(class UniformBuffer& ubo, unsigned int& offset_pointer) {};
	virtual ~Light() = default;
};


//------------------------------------DIRECTIONAL LIGHT-----------------------------------------/
struct DirectionalLight : public Light
{
	//-----current shader/GPU setup------
			//vec3 colour;                // 4N == 16
			//vec3 direction;             //12 bytes r 4
			//float ambinent;             //<<4
			//float diffuse;              // 4 bytes r 12
			//float specular;             // << 4 r 8
			//vec2 alignmentPadding;      //<< 8

	glm::vec3 direction;

	DirectionalLight(const glm::vec3& dir = glm::vec3(-1.0f, 1.0f, 1.0f), const glm::vec3& col = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);

	static long long int GetSimpleGPUSize()
	{
		return (2 * sizeof(glm::vec3) +	
				sizeof(int) + 
				3 * sizeof(float) + 
				sizeof(glm::vec2));
	}

	virtual void UpdateUniformBufferData(class UniformBuffer& ubo, unsigned int& offset_pointer) override;
};


//--------------------------------------POINT LIGHT---------------------------------------------/
struct PointLight : public Light
{
	//-----current shader/GPU setup------
		 //vec3 colour;
		 //float ambinent;
		 //vec3 position;
		 //float diffuse;
		 //vec3 attenuation;
		 //float specular;
		 //float enable;               
		 //vec3 alignmentPadding;     

	glm::vec3 position;

	//To prevent confusions, accessing attenuation from array
#define CONSTANT_ATT 0
#define LINEAR_ATT 1
#define QUARDRATIC_ATT 2

	//So as to store all attenuation contiguously in memory
	//constant -> 0
	//linear -> 1
	//quardratic ->2
	float attenuation[3];
	float shadow_far = 150.0f;
	PointLight(const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& col = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);

	static long long int GetSimpleGPUSize()
	{
		return (3 * sizeof(glm::vec3)+
				sizeof(int) + 
				4 * sizeof(float)+
				sizeof(glm::vec2));
	}

	virtual void UpdateUniformBufferData(class UniformBuffer& ubo, unsigned int& offset_pointer) override;

	float CalculateLightRadius(float threshold = 0.01f) const
	{
		return (-attenuation[LINEAR_ATT] +
			glm::sqrt(attenuation[LINEAR_ATT] * attenuation[LINEAR_ATT] - 4 * attenuation[QUARDRATIC_ATT] *
				(attenuation[CONSTANT_ATT] - 1.0f / threshold)))
			/ (2.0f * attenuation[QUARDRATIC_ATT]);
	}
};

//--------------------------------------SPOT LIGHT----------------------------------------------/

struct SpotLight : public PointLight
{
	glm::vec3 direction;

	//TO-DO: for now only use one float
	//		later convert to have
	//      innerCutoffAngle &
	//      outerCutoffAngle
	float innerCutoffAngle = 25.0f;
	float outerCutoffAngle = 33.0f;
	bool debug = false;
	SpotLight(const glm::vec3& dir = glm::vec3(-0.2f, -1.0f, -0.3f), float cutoffAngle = 33.0f, 
		const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& col = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);
};


