#include "Texture.h"
#include "RendererErrorAssertion.h"

#include "External Libs/stb_image/stb_image.h"
#include <iostream>

Texture::Texture()
	:m_Id(0), m_LocalBuffer(nullptr), m_Height(0),
	m_Width(0), m_BitDepth(0), m_RefCount(0)
{
}

Texture::Texture(const std::string& fileLoc)
	:m_Id(0), m_FileLocation(fileLoc),
	 m_LocalBuffer(nullptr), m_Height(0), 
	 m_Width(0), m_BitDepth(0), m_RefCount(0)
{
	LoadTexture(fileLoc);
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

bool Texture::LoadTexture(const std::string& fileLoc, TextureType type)
{
	m_FileLocation = fileLoc;
	m_type = type;
	//stbi_set_flip_vertically_on_load(0);
	int nrComponent;

	if (m_type == TextureType_NONE)
	{
		m_LocalBuffer = stbi_load(fileLoc.c_str(), &m_Width, &m_Height, &m_BitDepth, 4);
	}
	else
	{
		m_LocalBuffer = stbi_load(fileLoc.c_str(), &m_Width, &m_Height, &nrComponent, 0);
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




	if (m_type == TextureType_NONE)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	}
	else
	{
		//TO-Do: needs clean up
		GLenum format;
		switch (nrComponent)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			break;
		}

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer));
	}
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
