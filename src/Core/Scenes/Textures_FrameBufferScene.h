#pragma once
#include "Scene.h"

#include "Graphics/Shader.h"
#include "LearningOpenGLModel.h"

#include "Graphics/Meshes/SphereMesh.h"
#include "Graphics/Skybox.h"


class Texture_FrameBufferScene : public Scene
{
public:
	Texture_FrameBufferScene() = default;
	

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;
	
	
	~Texture_FrameBufferScene();

private:
	void CreateObjects();

	unsigned int FBO;
	unsigned int fboTex;

	unsigned int RBO;

	Shader screenShader;
	learning::ObjectVertex m_Quad;
	float screenTexSampleOffset = 300.0f; //inverted in shader
	bool specialScreenKernel = false;
	int specialKernelType = 0;


	//unsigned int cubeTexMap;
	class TextureCube* m_SkyboxMap = nullptr;
	Shader m_SkyboxShader;
	learning::ObjectVertex m_Skybox_vertex;

	Skybox m_DefaultSkybox;


	void DrawObjects(bool depth_test, bool get_rear);

	learning::ObjectVertex m_Cube;
	class Texture* m_CrateTex;
	Shader m_CrateShader;


	//Test skybox shader reflect on objects 
	Shader m_ObjectSampleReflect;

	SphereMesh m_Sphere;


	struct Transform
	{
		glm::vec3 pos = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

	}m_CubeWorTrans;

	learning::ObjectVertex m_Plane;
	Texture* m_PlaneTex;
	Transform m_PlaneWorTrans;

	//Transform m_SphereTrans;
	std::vector<Transform> m_SceneSphereTranforms;

	glm::vec3 m_ClearColourSecondPass = glm::vec3(0.3f);
	glm::vec3 m_PlayColourFBOTexture = glm::vec3(1.0f);
};