#include "TextureCube.h"
#include "RendererErrorAssertion.h"

#include "External Libs/stb_image/stb_image.h"
#include <iostream>

TextureCube::TextureCube()
	:m_Id(0), m_LocalBuffer(nullptr), m_Height(0),
	m_Width(0), m_BitDepth(0), m_RefCount(0)
{}

TextureCube::TextureCube(const std::vector<std::string>& fileLoc)
	:m_Id(0), m_FaceTextureFiles(fileLoc),
	m_LocalBuffer(nullptr), m_Height(0),
	m_Width(0), m_BitDepth(0), m_RefCount(0)
{
	LoadTexture(fileLoc);
}

TextureCube::~TextureCube()
{
}

void TextureCube::RegisterUse()
{
	m_RefCount++;
}

void TextureCube::UnRegisterUse()
{
	m_RefCount--;

	if (m_RefCount <= 0)
		Clear();
}

bool TextureCube::LoadTexture(const std::vector<std::string>& fileLoc)
{
	m_FaceTextureFiles = fileLoc;
	GLCall(glGenTextures(1, &m_Id));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id));

	int width,
		height,
		nrChannels;
	for (size_t i = 0; i < fileLoc.size(); i++)
	{
		//m_LocalBuffer = stbi_load(fileLoc[i].c_str(), &width, &height, &nrChannels, 0);
		unsigned char* data = stbi_load(fileLoc[i].c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
				width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		}
		else
		{
			std::cout << "[TEXTURE LOADING FAILED]: " << fileLoc[i].c_str()<< std::endl;
			//return false;
		}
		stbi_image_free(data);
	}


	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));



	//if (m_LocalBuffer)
		//stbi_image_free(m_LocalBuffer);

	return true;
}

void TextureCube::Activate(unsigned int slot) const
{
	//GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_Id));
}

void TextureCube::DisActivate() const
{
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void TextureCube::Clear()
{
}

