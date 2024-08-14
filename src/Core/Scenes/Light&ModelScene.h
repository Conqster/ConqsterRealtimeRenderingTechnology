#pragma once
#include "Scene.h"

#include "GameObject.h"
#include "Graphics/Lights/LightManager.h"
#include "Graphics/Shader.h"

#include "LearningOpenGLModel.h"





//Forward declares
class Texture;


class Light_ModelScene : public Scene
{
public:
	Light_ModelScene() = default;

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;

	~Light_ModelScene();
private:
	//TO-DO: Later have a setup light helper class 
//       As well as creat object 
	void CreateObjects();
	//void SetupLights();


	//void ProcessLight(NewShader& shader);
	//void ProcessInput(float delta_time);

	//for now
	Shader m_MainShaderProgram;

	glm::vec3 test_LightColour = glm::vec3(1.0f);
	glm::vec3 depth_fogColour = glm::vec3(1.0f);

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


	learning::L_Material test_Material{
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

	learning::light::L_Light test_Light
	{
		//glm::vec3(2.5f, 2.2f, -18.0f),

		glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(1.0f, 1.0f, 1.0f),

		false,
	};

	learning::light::L_DirectionLight dir_Light
	{
		//Light Base
		{
			glm::vec3(0.2f, 0.2f, 0.2f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(1.0f, 1.0f, 1.0f),

			false,
		},
		//Direction
		glm::vec3(-0.2f, -1.0f, -0.3f),
	};


	learning::light::L_PointLight pt_Lights[4];

	learning::light::L_SpotLight spot_light
	{
		//Point light base
		{
			//Light Base
			{
				glm::vec3(0.2f, 0.2f, 0.2f),
				glm::vec3(0.5f, 0.5f, 0.5f),
				glm::vec3(1.0f, 1.0f, 1.0f),

				true,
			},

			//Position 
			glm::vec3(0.0f),

			1.0f,     //con
			0.09f,    //lin
			0.032f,   //quad
		},

		//Direction 
		glm::vec3(-0.2f, -1.0f, -0.3f),

		//Cut off angles
		12.5f,	//inner
		17.5f,	//outer
	};


	glm::vec3 cubePositions[10] = {
				glm::vec3(0.0f,  0.0f,  0.0f),
				glm::vec3(2.0f,  5.0f, -15.0f),
				glm::vec3(-1.5f, -2.2f, -2.5f),
				glm::vec3(-3.8f, -2.0f, -12.3f),
				glm::vec3(2.4f, -0.4f, -3.5f),
				glm::vec3(-1.7f,  3.0f, -7.5f),
				glm::vec3(1.3f, -2.0f, -2.5f),
				glm::vec3(1.5f,  2.0f, -2.5f),
				glm::vec3(1.5f,  0.2f, -1.5f),
				glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	float newMaterialEmissionIntensity = 5.0f;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;

	class CubeMesh* cube_Test;
	Texture* cube_DiffuseMap_Test;
	Texture* cube_SpecularMap_Test;
	Texture* cube_EmissionMap_Test;

	unsigned int cubeVBO, cubeVAO;

	bool useNewModel = true;


	learning::model::L_Model newModel;
	Shader modelShader;

	void AdditionalShader(const ShaderFilePath& shader_file, const glm::mat4& viewProj);
};