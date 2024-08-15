#include "Framebuffer.h"

#include "..\RendererErrorAssertion.h"
#include <iostream>

Framebuffer::Framebuffer() : m_Width(500), m_Height(500),
				m_ID(0), m_RenderbufferID(0), m_TextureID(0)
{}

Framebuffer::Framebuffer(unsigned int width, unsigned int height) : m_Width(width),
			m_Height(height), m_ID(0), m_RenderbufferID(0), m_TextureID(0)
{
	Generate();
}

Framebuffer::~Framebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &m_ID);

	glDeleteRenderbuffers(1, &m_RenderbufferID);

	glDeleteTextures(1, &m_TextureID);        //delete frame texture/ texture attached to frame buffer
}

bool Framebuffer::Generate()
{
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	///////////////////////////////////////////////////////////////////////
	// create colour attachment texture for frame buffer
	///////////////////////////////////////////////////////////////////////
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//attach this new texture(fboTex) to the framebuffer FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);


	///////////////////////////////////////////////////////////////////////
	// create a render buffer object for depth and stencil 
	///////////////////////////////////////////////////////////////////////
	glGenRenderbuffers(1, &m_RenderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[FRAMEBUFFER ERROR]: Framebuffer did not complete!!!!\n";
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		return false;
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return true;
}

bool Framebuffer::Generate(unsigned int width, unsigned int height)
{
	m_Width = width;
	m_Height = height;
	return Generate();
}

void Framebuffer::Bind()
{
	glViewport(0, 0, m_Width, m_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void Framebuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::BindTexture(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

