#include "Texture.h"
#include "RendererErrorAssertion.h"

#include "External Libs/stb_image/stb_image.h"
#include <iostream>


static GLint OpenGLTexFormat(TextureFormat format)
{
	switch (format)
	{
		case TextureFormat::RGB: return GL_RGB;
		case TextureFormat::RGBA: return GL_RGBA;
		case TextureFormat::SRGB: return GL_SRGB;
		case TextureFormat::SRGBA: return GL_SRGB_ALPHA;
		case TextureFormat::RED: return GL_RED;
	}

	std::cout << "[TEXTURE]: Format not supported yet !!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
	return GL_RGB;
}

Texture::Texture()
	:m_Id(0), m_LocalBuffer(nullptr), m_Height(0),
	m_Width(0), m_BitDepth(0), m_RefCount(0),
	m_TexFormat(TextureFormat::RGBA), m_TexType(TextureType::TextureType_NONE)
{
}

Texture::Texture(const std::string& fileLoc, bool flip,  TextureFormat format, TextureType type)
	:m_Id(0), m_FileLocation(fileLoc),
	 m_LocalBuffer(nullptr), m_Height(0), 
	 m_Width(0), m_BitDepth(0), m_RefCount(0),
	 m_TexFormat(format), m_TexType(type)
{
	LoadTexture(fileLoc, flip, m_TexFormat);
}

Texture::~Texture()
{
	//work around as the texture is stored as a variable 
	// it does not go out of scope before glfwTerminate -> window 
	// which creates an error >> moved to Clear()
	//Clear();
}

void Texture::RegisterUse()
{
	m_RefCount++;
}

void Texture::UnRegisterUse()
{
	m_RefCount--;

	if(m_RefCount <= 0)
		Clear();
}

bool Texture::LoadTexture(const std::string& fileLoc, bool flip , TextureFormat format, TextureType type)
{
	m_FileLocation = fileLoc;
	m_TexType = type;
	m_TexFormat = format;
	stbi_set_flip_vertically_on_load(flip);
	int desire_channel = (type == TextureType::TextureType_NONE) ? 4 : 0;
		//m_LocalBuffer = stbi_load(fileLoc.c_str(), &m_Width, &m_Height, &m_BitDepth, desire_channel);

	if (type == TextureType::TextureType_NONE)
	{
		m_LocalBuffer = stbi_load(fileLoc.c_str(), &m_Width, &m_Height, &m_BitDepth, 4); 

	}
	else
	{
		m_LocalBuffer = stbi_load(fileLoc.c_str(), &m_Width, &m_Height, &m_BitDepth, 0);
		//m_LocalBuffer = stbi_load(fileLoc.c_str(), &m_Width, &m_Height, &channel, 0);

	}

	if (!m_LocalBuffer)
	{
		std::cout << "Failed to find: " << fileLoc.c_str() << std::endl;
		return false;
	}

	GLCall(glGenTextures(1, &m_Id));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_Id));



	if (true)
	{
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	}

	if (false)
	{
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	}

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	if (type == TextureType::TextureType_NONE)
	{
		//GLCall(glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTexFormat(m_TexFormat), m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_LocalBuffer));
		//GLCall(glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTexFormat(m_TexFormat), m_Width, m_Height, 0, OpenGLTexFormat(m_TexFormat), GL_UNSIGNED_BYTE, m_LocalBuffer));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTexFormat(m_TexFormat), m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
		//GLCall(glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTexFormat(m_TexFormat), m_Width, m_Height, 0, OpenGLTexFormat((TextureFormat)m_BitDepth), GL_UNSIGNED_BYTE, m_LocalBuffer));

	}
	else
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTexFormat(m_TexFormat), m_Width, m_Height, 0, OpenGLTexFormat((TextureFormat)m_BitDepth), GL_UNSIGNED_BYTE, m_LocalBuffer));

	}
	//GLCall(glTexImage2D(GL_TEXTURE_2D, 0, OpenGLTexFormat(m_TexFormat), m_Width, m_Height, 0, OpenGLTexFormat(m_TexFormat), GL_UNSIGNED_BYTE, m_LocalBuffer));
	
	glGenerateMipmap(GL_TEXTURE_2D);

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if(m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);

	return true;
}


void Texture::Activate(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_Id));
}

void Texture::DisActivate() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::Clear()
{
	std::cout << "Cleared texture\n";
	GLCall(glDeleteTextures(1, &m_Id));

	m_Id = 0;
	m_Width = 0;
	m_Height = 0;
	m_BitDepth = 0;
	m_FileLocation = " ";
	m_LocalBuffer = nullptr;
}
