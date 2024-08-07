#pragma once
#include "GL/glew.h"
#include "Camera.h"
#include "Window.h"

	class Scene
	{
	private:
		//Scene Model Loader
		//Object loader
		//Light Setup Object

		//Camera* m_Camera = nullptr;
	public:
		Scene() = default;

		virtual void SetWindow(Window* window);
		inline Camera* const GetCamera() const { return m_Camera; }

		virtual void OnInit();
		virtual void OnUpdate(float delta_time);
		virtual void OnRender();
		virtual void OnRenderUI();

		virtual void OnDestroy();

		~Scene();
	protected:
		Camera* m_Camera = nullptr;
		Window* window = nullptr;
	};