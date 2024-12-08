#pragma once
#include "glm/glm.hpp"
#include "RendererErrorAssertion.h"

enum class DepthMode
{
	NEVER = GL_NEVER,
	LEEQUAL = GL_LEQUAL,
};

enum class BlendFactors
{
	SRC_ALPHA = GL_SRC_ALPHA,
	ONE_MINUS_SCR_A = GL_ONE_MINUS_SRC_ALPHA,
};


class RenderCommand
{
public:

	static void Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

	static void ClearColour(glm::vec3& colour);
	static void ClearColour(glm::vec4& colour);
	static void ClearColour(float r, float g, float b, float a = 1.0f);
	static void Clear();
	static void ClearColourOnly();
	static void ClearDepthOnly();
	static void ClearStencilOnly();

	static void CullFront();
	static void CullBack();


	//config
	static void EnableDepthTest();
	static void DisableDepthTest();
	static void EnableFaceCull();
	static void DisableFaceCull();
	
	static void EnableBlend();
	static void DisableBlend();
	static void BlendFactor(BlendFactors src_factor, BlendFactors des_factor);

	//FUNC
	static void DepthTestMode(DepthMode mode);
	static void DepthMask(bool mask);
};