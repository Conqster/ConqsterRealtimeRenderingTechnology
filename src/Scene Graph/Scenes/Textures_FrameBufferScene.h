#pragma once
#include "Scene.h"

#include "Renderer/Renderer.h"
#include "Renderer/Skybox.h"
#include "Renderer/Meshes/Mesh.h"

#include "Renderer/ObjectBuffer/Framebuffer.h"


class Texture_FrameBufferScene : public Scene
{
public:
	Texture_FrameBufferScene() = default;
	
	//static void RegisterSceneImpl(const char* name);

	virtual void OnInit(Window* window) override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnRender() override;
	virtual void OnRenderUI() override;

	virtual void OnDestroy() override;
	
	
	~Texture_FrameBufferScene();

private:
	void CreateObjects();

	Framebuffer m_Framebuffer;

	bool m_FrameCaptureRear = false;
	bool m_PerfromDepthTest = true;

	Shader screenShader;
	LearnVertex m_Quad;
	float screenTexSampleOffset = 300.0f; //inverted in shader
	bool specialScreenKernel = false;
	int specialKernelType = 0;


	//unsigned int cubeTexMap;
	Skybox m_DefaultSkybox;


	void DrawObjects(bool depth_test, bool get_rear);

	LearnVertex m_Cube;
	class Texture* m_CrateTex;
	Shader m_CrateShader;


	//Test skybox shader reflect on objects 
	Shader m_ObjectSampleReflect;

	Mesh m_Sphere;
	Texture* m_SphereTex;
	float m_SkyboxInfluencity = 0.5f;
	glm::vec3 m_TextureColour = glm::vec3(0.0f, 0.0f, 1.0f);


	struct Transform
	{
		glm::vec3 pos = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);

	}m_CubeWorTrans;

	LearnVertex m_Plane;
	Texture* m_PlaneTex;
	Transform m_PlaneWorTrans;

	//Transform m_SphereTrans;
	std::vector<Transform> m_SceneSphereTranforms;

	glm::vec3 m_ClearColourSecondPass = glm::vec3(0.3f);
	glm::vec3 m_PlayColourFBOTexture = glm::vec3(1.0f);
};