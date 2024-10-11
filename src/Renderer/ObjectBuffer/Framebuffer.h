#pragma once


class Framebuffer
{
public:
	Framebuffer();
	Framebuffer(unsigned int width, unsigned int height);
	~Framebuffer();

	bool Generate();
	bool Generate(unsigned int width, unsigned int height);



	void Bind();
	void UnBind();
	void Delete();

	void BindTexture(unsigned int slot = 0);

private:
	unsigned int m_Width,
				m_Height;

	unsigned int m_ID,
				 m_RenderbufferID,
				 m_TextureID;
};
