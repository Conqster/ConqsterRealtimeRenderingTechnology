#pragma once



class MSAA
{
public:
	MSAA();
	MSAA(unsigned int width, unsigned int height);

	bool Generate();
	bool Generate(unsigned int width, unsigned int height);

	void Bind();
	void Blit();
	void UnBind();
	void BindTexture(unsigned int slot = 0);

	void Delete();
	~MSAA();
private:
	unsigned int m_Width,
			     m_Height;

	unsigned int m_ID,
				 m_RenderbufferID,
				 m_TextureID;

	unsigned int m_MultiSampleTex,
				 m_IntermidateFBO;
};