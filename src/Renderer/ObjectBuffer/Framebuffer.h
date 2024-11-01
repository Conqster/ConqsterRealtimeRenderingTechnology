#pragma once
#include "glm/glm.hpp"

enum FBO_Format
{
	RGB,
	RGBA16F
};


class Framebuffer
{
public:
	Framebuffer();
	Framebuffer(unsigned int width, unsigned int height, FBO_Format i_format = FBO_Format::RGB);
	~Framebuffer();

	bool Generate(FBO_Format i_format = FBO_Format::RGB);
	bool Generate(unsigned int width, unsigned int height, FBO_Format i_format = FBO_Format::RGB);



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
};
