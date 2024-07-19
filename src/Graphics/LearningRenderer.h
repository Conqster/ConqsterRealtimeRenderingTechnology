#pragma once
#include "RendererBase.h"


struct TestMaterial
{
	/*Good setup for Materials 
	* 3 maps :- Diffuse, Specular & Emssion 
	* shiniess value of specular 
	* strength value of emission
	* Colour of Emission
	*/
	int diffuseMap;
	int specularMap;
	int emissionMap;
	/*float*/ int shiniess;

	//stored as int foe simplity
	bool invertSpecularMap;
	glm::vec3 emissionColour;
	float emissionStrength;
	bool emit;
};


struct TestLight
{
	/*Still trying try to decide what 
	* is best for light, FOR: 
	* FOR:- Ambient, diffuse & Specular
	* To have seperate colour (vec3) OR
	* TO only intensity that influence a single colour 
	*/
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

//Forward declares
class Texture;

class LearningRenderer : public RendererBase
{
public:
	LearningRenderer();
	//MainRenderer(unsigned int width, unsigned int height);
	LearningRenderer(Window& use_window);


	void Render(Camera& camera);

	glm::vec3 test_LightColour = glm::vec3(1.0f);

	float test_GlobalAmbientStrength = 1.0f;
	float test_GlobalDiffuseStrength = 1.0f;
	float test_GlobalSpecularStrength = 1.0f; // 0.5f;


	//Silver 
	//TestMaterial test_Material{
	//	glm::vec3(0.19225),
	//	glm::vec3(0.50754),
	//	glm::vec3(0.508273),
	//	32
	//};


	TestMaterial test_Material{
		//glm::vec3(1.0f, 0.5f, 0.31f),
		//glm::vec3(1.0f, 0.5f, 0.31f),
		0,  //slot or location of the diffuse texture map in the GPU
		1,  //slot or location of the specular texture map in the GPU
		2,  //slot or location of the emission texture map in the GPU
		32 * 2,  //shiniess
		false,       //invert specular map
		glm::vec3(0.0f, 1.0f, 0.0f), // emissiion coloe
		1.0f,   //emission strength
		false,  //emit
	};

	TestLight test_Light
	{
		glm::vec3(2.5f, 2.2f, -18.0f),

		glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(1.0f, 1.0f, 1.0f),
	};

private: 
	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;

	class CubeMesh* cube_Test;
	Texture* cube_DiffuseMap_Test;
	Texture* cube_SpecularMap_Test;
	Texture* cube_EmissionMap_Test;

	unsigned int cubeVBO, cubeVAO;


	void CreateObject();
};