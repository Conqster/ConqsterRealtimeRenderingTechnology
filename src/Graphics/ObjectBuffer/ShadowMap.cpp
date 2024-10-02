#include "ShadowMap.h"
#include "../RendererErrorAssertion.h"

void ShadowMap::Generate()
{
	glGenFramebuffers(1, &m_Id);

	glGenTextures(1, &m_TexMapId);
	glBindTexture(GL_TEXTURE_2D, m_TexMapId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColour[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);

	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TexMapId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::Write()
{
	glViewport(0, 0, m_Width, m_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
}

void ShadowMap::Read(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_TexMapId);
}

void ShadowMap::BindMapTexture(unsigned int slot)
{

}

void ShadowMap::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
