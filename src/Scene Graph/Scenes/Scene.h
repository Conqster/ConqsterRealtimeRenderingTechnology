#pragma once
#include "GL/glew.h"
#include "Camera.h"
#include "Window.h"


#include "Renderer/SceneRenderer.h"


struct LearnVertex
{
	unsigned int VAO;
	unsigned int VBO;
};

	
	class Scene
	{
	public:
		Scene() = default;
		virtual void SetWindow(Window* window);
		inline Camera* const GetCamera() const { return m_Camera; }

		virtual void OnInit(Window* window);
		virtual void OnUpdate(float delta_time);
		virtual void OnRender();
		virtual void OnRenderUI();

		virtual void OnDestroy();

		virtual ~Scene() = default;
	protected:
		Camera* m_Camera = nullptr;
		Window* window = nullptr;


		CRRT::SceneRenderer m_SceneRenderer;

		glm::vec3 m_ClearScreenColour = glm::vec3(0.1f, 0.1f, 0.1f);
		class Texture* blank_tex = nullptr;
	};
