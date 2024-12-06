#include "Framebuffer.h"

#include "..\RendererErrorAssertion.h"
#include <iostream>

static GLint OpenGLFormat(FBO_Format format)
{
	switch (format)
	{
		case FBO_Format::RGB: return GL_RGB;
		case FBO_Format::RGBA: return GL_RGBA;

		case FBO_Format::RGB16F: return GL_RGB16F;
		case FBO_Format::RGBA16F: return GL_RGBA16F;

		case FBO_Format::RGBA32F: return GL_RGBA32F;

		case FBO_Format::RGBA16: return GL_RGBA16;
	}

	std::cout << "[TEXTURE]: Format not supported yet !!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
	return GL_RGB;
}

Framebuffer::Framebuffer() : m_Width(500), m_Height(500),m_ID(0), m_RenderbufferID(0), 
							 m_TextureID(0), m_InternalFormat(FBO_Format::RGB)
{}

Framebuffer::Framebuffer(unsigned int width, unsigned int height, FBO_Format i_format) : m_Width(width),
			m_Height(height), m_ID(0), m_RenderbufferID(0), m_TextureID(0), m_InternalFormat(i_format)
{
	Generate(i_format);
}

Framebuffer::~Framebuffer()
{
	Delete();
}

bool Framebuffer::Generate(FBO_Format i_format)
{
	m_InternalFormat = i_format;

	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	///////////////////////////////////////////////////////////////////////
	// create colour attachment texture for frame buffer
	///////////////////////////////////////////////////////////////////////
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	//glTexImage2D(GL_TEXTURE_2D, 0, OpenGLFormat(i_format), m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, OpenGLFormat(i_format), m_Width, m_Height, 0, GL_RGB, GL_FLOAT, NULL);

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

bool Framebuffer::Generate(unsigned int width, unsigned int height, FBO_Format i_format)
{
	m_Width = width;
	m_Height = height;
	return Generate(i_format);
}

bool Framebuffer::ResizeBuffer(unsigned int width, unsigned int height)
{
	m_Width = width;
	m_Height = height;
	//resize texture 
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, OpenGLFormat(m_InternalFormat), m_Width, m_Height, 0, GL_RGB, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	//resize render buffer
	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[FRAMEBUFFER ERROR]: Framebuffer did not complete!!!!\n";
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		return false;
	}
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	return true;
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

void Framebuffer::Delete()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &m_ID);

	glDeleteRenderbuffers(1, &m_RenderbufferID);

	glDeleteTextures(1, &m_TextureID);        //delete frame texture/ texture attached to frame buffer
}

void Framebuffer::BindTexture(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
}



//----------------------------------------------Multiple Render Targets MRT----------------------/
MRTFramebuffer::MRTFramebuffer() : m_Width(500), m_Height(500), m_ID(0), m_RenderbufferID(0),
								   m_ColourAttachmentCount(0)//, m_InternalFormat(FBO_Format::RGB)
{
}

MRTFramebuffer::MRTFramebuffer(unsigned int width, unsigned int height, unsigned int count, FBO_Format i_format) : m_Width(width),
m_Height(height), m_ColourAttachmentCount(count), m_ID(0)//, m_RenderbufferID(0)
{
	Generate(count, i_format);
}

MRTFramebuffer::~MRTFramebuffer()
{
	Delete();
}

bool MRTFramebuffer::Generate(unsigned int count, FBO_Format i_format)
{
	//m_InternalFormat = i_format;
	imgFormatConfig.push_back({ i_format, GL_FLOAT });
	m_ColourAttachmentCount = count;
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	///////////////////////////////////////////////////////////////////////
	// create colour attachment texture for frame buffer
	///////////////////////////////////////////////////////////////////////
	colourAttachments.reserve(count);
	//Quick hack 
	for (unsigned int i = 0; i < count; i++)
		colourAttachments.emplace_back(i);
	glGenTextures(count, &colourAttachments[0]);

	for (unsigned int i = 0; i < count; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colourAttachments[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, OpenGLFormat(i_format), m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_LINEAR*/GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, /*GL_LINEAR*/GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colourAttachments[i], 0);
	}


	std::vector<unsigned int> attachment;
	attachment.reserve(count);
	for (unsigned int i = 0; i < count; i++)
	{
		attachment.emplace_back(GL_COLOR_ATTACHMENT0 + i);
	}
	glDrawBuffers(count, &attachment[0]);

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

bool MRTFramebuffer::Generate(unsigned int width, unsigned int height, std::vector<FBO_TextureImageConfig> img_config)
{
	//for now 
	if (img_config.size() <= 0)
	{
		printf("[MRT FRAMEBUFFER ERROR]:img_config is empty, img_config is used to determine buffer count!!!!\n");
		return false;
	}

	m_Width = width;
	m_Height = height;
	//m_InternalFormat = img_config[0].format;
	imgFormatConfig = img_config;
	unsigned int count = img_config.size();
	m_ColourAttachmentCount = count;
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	///////////////////////////////////////////////////////////////////////
	// create colour attachment texture for frame buffer
	///////////////////////////////////////////////////////////////////////
	colourAttachments.reserve(count);

	for (unsigned int i = 0; i < count; i++)
	{
		//create a colour attachment for i buffer
		colourAttachments.emplace_back(i);
		//generate texture for colour attachment buffer
		glGenTextures(1, &colourAttachments.back());

		glBindTexture(GL_TEXTURE_2D, colourAttachments[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, OpenGLFormat(img_config[i].internalFormat), m_Width, m_Height, 0, OpenGLFormat(img_config[i].format), img_config[i].imgDataType, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colourAttachments[i], 0);
	}


	std::vector<unsigned int> attachment;
	attachment.reserve(count);
	for (unsigned int i = 0; i < count; i++)
	{
		attachment.emplace_back(GL_COLOR_ATTACHMENT0 + i);
	}
	glDrawBuffers(count, &attachment[0]);

	///////////////////////////////////////////////////////////////////////
	// create a render buffer object for depth and stencil 
	///////////////////////////////////////////////////////////////////////
	glGenRenderbuffers(1, &m_RenderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[FRAMEBUFFER ERROR]: Framebuffer did not complete!!!!\n";
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		return false;
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return true;
}

bool MRTFramebuffer::Generate(unsigned int width, unsigned int height, unsigned int count, FBO_Format i_format)
{
	m_Width = width;
	m_Height = height;
	m_ColourAttachmentCount = count;
	return Generate(count, i_format);
}

bool MRTFramebuffer::ResizeBuffer(unsigned int width, unsigned int height)
{
	m_Width = width;
	m_Height = height;
	//resize textures
	for (unsigned int i = 0; i < m_ColourAttachmentCount; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colourAttachments[i]);
		if(i < imgFormatConfig.size())
			glTexImage2D(GL_TEXTURE_2D, 0, OpenGLFormat(imgFormatConfig[i].internalFormat), m_Width, m_Height, 0, OpenGLFormat(imgFormatConfig[i].format), imgFormatConfig[i].imgDataType, NULL);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, OpenGLFormat(imgFormatConfig[0].internalFormat), m_Width, m_Height, 0, GL_RGBA, imgFormatConfig[0].imgDataType, NULL);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	//resize render buffer
	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[FRAMEBUFFER ERROR]: Framebuffer did not complete!!!!\n";
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		return false;
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return true;
}

void MRTFramebuffer::Bind()
{
	glViewport(0, 0, m_Width, m_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

}

void MRTFramebuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MRTFramebuffer::Delete()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &m_ID);

	glDeleteRenderbuffers(1, &m_RenderbufferID);

	//glDeleteTextures(3, colourAttachments);        //delete frame texture/ texture attached to frame buffer
	glDeleteTextures(m_ColourAttachmentCount, &colourAttachments[0]);        //delete frame texture/ texture attached to frame buffer
}

/// <summary>
/// Would return last colour attachment, 
/// <para>if idx is greater than available colour attachment</para>
/// </summary>
/// <param name="idx"></param>
/// <param name="slot"></param>
void MRTFramebuffer::BindTextureIdx(unsigned int idx, unsigned int slot)
{
	if (idx > m_ColourAttachmentCount)
	{
		printf("idx pass colour attachment count/size :%d. return last colour attachment.\n", m_ColourAttachmentCount);
		idx = m_ColourAttachmentCount;
	}
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, colourAttachments[idx]);
}


