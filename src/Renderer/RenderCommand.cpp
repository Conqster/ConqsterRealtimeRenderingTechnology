#include "RenderCommand.h"

void RenderCommand::Viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	glViewport(x, y, width, height);
}

void RenderCommand::ClearColour(glm::vec3& colour)
{
	glClearColor(colour.r, colour.g, colour.b, 1.0f);
}

void RenderCommand::ClearColour(glm::vec4& colour)
{
	glClearColor(colour.r, colour.g, colour.b, colour.a);
}

void RenderCommand::ClearColour(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void RenderCommand::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderCommand::ClearDepthOnly()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderCommand::CullFront()
{
	glCullFace(GL_FRONT);
}

void RenderCommand::CullBack()
{
	glCullFace(GL_BACK);
}

void RenderCommand::EnableDepthTest()
{
	glEnable(GL_DEPTH_TEST);
}

void RenderCommand::DisableDepthTest()
{
	glDisable(GL_DEPTH_TEST);
}

void RenderCommand::EnableFaceCull()
{
	glEnable(GL_CULL_FACE);
}

void RenderCommand::DisableFaceCull()
{
	glDisable(GL_CULL_FACE);
}

void RenderCommand::EnableBlend()
{
	glEnable(GL_BLEND);
}

void RenderCommand::DisableBlend()
{
	glDisable(GL_BLEND);
}

void RenderCommand::BlendFactor(BlendFactors src_factor, BlendFactors des_factor)
{
	glBlendFunc((GLenum)src_factor, (GLenum)des_factor);
}

void RenderCommand::DepthTestMode(DepthMode mode)
{
	glDepthFunc((GLenum)mode);
}

void RenderCommand::DepthMask(bool mask)
{
	glDepthMask((GLboolean)mask);
}
