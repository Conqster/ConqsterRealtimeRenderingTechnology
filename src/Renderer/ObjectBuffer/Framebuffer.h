#pragma once
#include "glm/glm.hpp"
#include <vector>

enum class FBO_Format
{
	RGB,
	RGBA,

	RGB16F,
	RGBA16F,

	RGB32F,
	RGBA32F,

	RGBA16,
};




class Framebuffer
{
public:
	Framebuffer();
	Framebuffer(unsigned int width, unsigned int height, FBO_Format i_format = FBO_Format::RGB);
	~Framebuffer();

	bool Generate(FBO_Format i_format = FBO_Format::RGB);
	bool Generate(unsigned int width, unsigned int height, FBO_Format i_format = FBO_Format::RGB);

	bool ResizeBuffer(unsigned int width, unsigned int height);

	void Bind();
	void UnBind();
	void Delete();

	void BindTexture(unsigned int slot = 0);

	inline glm::vec2 GetSize() { return glm::vec2(m_Width, m_Height); }
	inline unsigned int GetColourAttachment() { return m_TextureID; }


private:
	unsigned int m_Width,
				m_Height;

	unsigned int m_ID,
				 m_RenderbufferID,
				 m_TextureID;

	FBO_Format m_InternalFormat;
};



//----------------------------------------------Multiple Render Targets MRT----------------------/

struct FBO_TextureImageConfig
{
	FBO_Format internalFormat = FBO_Format::RGBA16F;
	//change this later
	unsigned int imgDataType = 0x1406; //GL_FLOAT
	FBO_Format format = FBO_Format::RGBA;
};

class MRTFramebuffer
{
public:
	MRTFramebuffer();
	MRTFramebuffer(unsigned int width, unsigned int height, unsigned int count = 2, FBO_Format i_format = FBO_Format::RGB);
	~MRTFramebuffer();

	bool Generate(unsigned int count = 2, FBO_Format i_format = FBO_Format::RGB);
	bool Generate(unsigned int width, unsigned int height, std::vector<FBO_TextureImageConfig> img_config);
	bool Generate(unsigned int width, unsigned int height, unsigned int count = 2, FBO_Format i_format = FBO_Format::RGB);

	bool ResizeBuffer(unsigned int width, unsigned int height);

	void Bind();
	void UnBind();
	void BlitDepth();
	void Delete();

	void BindTextureIdx(unsigned int idx, unsigned int slot = 0);

	inline glm::vec2 GetSize() { return glm::vec2(m_Width, m_Height); }
	inline unsigned int GetColourAttachmentCount() { return m_ColourAttachmentCount; }
	inline unsigned int GetColourAttachment(unsigned int idx) { return colourAttachments[idx]; }


private:
	unsigned int m_Width,
				 m_Height;

	unsigned int m_ID,
				 m_RenderbufferID;

	unsigned int m_ColourAttachmentCount = 0;
	
	std::vector<unsigned int> colourAttachments;
	std::vector<FBO_TextureImageConfig> imgFormatConfig;
	//FBO_Format m_InternalFormat;
};
