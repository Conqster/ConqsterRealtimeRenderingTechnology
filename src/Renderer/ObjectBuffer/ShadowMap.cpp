#include "ShadowMap.h"
#include "../RendererErrorAssertion.h"
#include <iostream>

void ShadowMap::Generate(unsigned int width, unsigned int height)
{
	m_Width = width;
	m_Height = height;
	Generate();
}

void ShadowMap::Generate()
{
	GLCall(glGenFramebuffers(1, &m_Id));

	GLCall(glGenTextures(1, &m_TexMapId));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TexMapId));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	float borderColour[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Id));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TexMapId, 0));
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("[FRAMEBUFFER ERROR]: Shadow map FBO did not complete!!!!\n");
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		return;
	}

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
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

void ShadowMap::UnBindMap()
{
	GLCall(glBindTexture(GL_TEXTURE, 0));
}

void ShadowMap::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//---------------------------------Shadow Cube---------------------------------/


void ShadowCube::Generate()
{
	GLCall(glGenFramebuffers(1, &m_Id));

	GLCall(glGenTextures(1, &m_TexMapId));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_TexMapId));

	for (unsigned int i = 0; i < 6; ++i)
	{
		GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_DEPTH_COMPONENT, m_Width, m_Height,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	}

	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));


	//bind directly to the depth sttachment, glFramebufferTexture
	//using geometry shader for just a single pass instead of six
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_Id));
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_TexMapId, 0));
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("[FRAMEBUFFER ERROR]:  Shadow cube map FBO did not complete!!!!\n");
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		return;
	}

	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowCube::Read(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TexMapId);
}

void ShadowCube::UnBindMap()
{
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}
