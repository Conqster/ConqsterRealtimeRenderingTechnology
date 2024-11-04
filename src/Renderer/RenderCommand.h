#pragma once
#include "glm/glm.hpp"
#include "RendererErrorAssertion.h"

enum DepthMode
{
	NEVER = GL_NEVER,
	LEEQUAL = GL_LEQUAL,
};


class RenderCommand
{
public:

	static void Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

	static void ClearColour(glm::vec3& colour);
	static void ClearColour(glm::vec4& colour);
	static void ClearColour(float r, float g, float b, float a = 1.0f);
	static void Clear();
	static void ClearDepthOnly();

	static void CullFront();
	static void CullBack();


	//config
	static void EnableDepthTest();
	static void DisableDepthTest();
	static void EnableFaceCull();
	static void DisableFaceCull();

	//FUNC
	static void DepthTestMode(DepthMode mode);
};