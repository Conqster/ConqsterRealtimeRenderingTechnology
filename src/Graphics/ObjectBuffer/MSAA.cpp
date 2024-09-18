#include "MSAA.h"
#include "..\RendererErrorAssertion.h"

#include <iostream>

MSAA::MSAA() : m_Width(500), m_Height(500), m_ID(0), m_RenderbufferID(0), 
			m_TextureID(0), m_MultiSampleTex(0), m_IntermidateFBO(0)
{}

MSAA::MSAA(unsigned int width, unsigned int height) : m_Width(width), m_Height(height), m_ID(0), 
			m_RenderbufferID(0), m_TextureID(0), m_MultiSampleTex(0), m_IntermidateFBO(0)
{
	Generate(width, height);
}

bool MSAA::Generate(unsigned int width, unsigned int height)
{
	m_Width = width;
	m_Height = height;
	return Generate();
}

bool MSAA::Generate()
{
	GLCall(glGenFramebuffers(1, &m_ID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));

	//create a multisampled colour attachment tecxture 
	GLCall(glGenTextures(1, &m_MultiSampleTex));
	GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MultiSampleTex));
	GLCall(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, m_Width, m_Height, GL_TRUE));
	GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_MultiSampleTex, 0));
	//Create a renderobject object for depth & stencil attachments
	GLCall(glGenRenderbuffers(1, &m_RenderbufferID));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID));
	GLCall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, m_Width, m_Height));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("[ERROR MSAA FRAMEBUFFER]: Framebuffer is not complete!\n");
		return false;
	}

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));


	/////////////////////////////////////
	// MIGHT CHANGE OR REPLACE LATER
	/////////////////////////////////////
	//configure second post-processing framebuffer
	GLCall(glGenFramebuffers(1, &m_IntermidateFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_IntermidateFBO));
	//create colour attachment
	GLCall(glGenTextures(1, &m_TextureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("[ERROR INTERMEDIATE FRAMEBUFFER]: intermediate Framebuffer is not complete!\n");
		return false;
	}

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	return true;
}



void MSAA::Bind()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
}

void MSAA::Blit()
{
	GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ID));
	GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_IntermidateFBO));
	GLCall(glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, m_Width, m_Height, GL_COLOR_BUFFER_BIT, GL_NEAREST));

}

void MSAA::UnBind()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void MSAA::BindTexture(unsigned int slot)
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID));
}

void MSAA::Delete()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &m_ID);

	glDeleteRenderbuffers(1, &m_RenderbufferID);

	glDeleteTextures(1, &m_TextureID);        //delete frame texture/ texture attached to frame buffer

	GLCall(glDeleteFramebuffers(1, &m_IntermidateFBO));
	GLCall(glDeleteTextures(1, &m_MultiSampleTex));
}

MSAA::~MSAA()
{
	Delete();
}

