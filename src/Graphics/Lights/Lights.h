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

	LightType type = lightType_NONE;

	const char* LightTypeToString();

	Light(const glm::vec3& colour = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);
	virtual void UpdateUniformBufferData(class UniformBuffer& ubo, long long int& offset_pointer) {};
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

	static long long int GetGPUSize()
	{
		return (sizeof(glm::vec4) +		//colour
				sizeof(glm::vec3)+		//direction
				sizeof(float) +			//ambient
				sizeof(float)+			//diffuse
				sizeof(float)+			//specular
				sizeof(glm::vec2));		//alignment	
	}

	virtual void UpdateUniformBufferData(class UniformBuffer& ubo, long long int& offset_pointer) override;
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

	glm::vec3 position;

	//So as to store all attenuation contiguously in memory
	//constant
	//linear
	//quardratic
	float attenuation[3];
	PointLight(const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& col = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);

	static long long int GetGPUSize()
	{
		return (sizeof(glm::vec3) +		//colour
				sizeof(float) +			//ambinent
				sizeof(glm::vec3) +		//position
				sizeof(float) +			//diffuse
				sizeof(float) +			//spec
				sizeof(glm::vec3));		//attenuations
	}

	virtual void UpdateUniformBufferData(class UniformBuffer& ubo, long long int& offset_pointer) override;
};

//--------------------------------------SPOT LIGHT----------------------------------------------/

struct SpotLight : public PointLight
{
	glm::vec3 direction;

	//TO-DO: for now only use one float
	//		later convert to have
	//      innerCutoffAngle &
	//      outerCutoffAngle
	float innerCutoffAngle = 12.5f;
	float outerCutoffAngle = 17.5f;
	SpotLight(const glm::vec3& dir = glm::vec3(-0.2f, -1.0f, -0.3f), float cutoffAngle = 17.5f, 
		const glm::vec3& pos = glm::vec3(0.0f), const glm::vec3& col = glm::vec3(1.0f), float amb_inten = 0.5f, float diff_inten = 0.6f);
};


