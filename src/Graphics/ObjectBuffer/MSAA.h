#pragma once



class MSAA
{
public:
	MSAA();
	MSAA(unsigned int width, unsigned int height, unsigned int sample = 4);

	bool Generate();
	bool Resize(unsigned int width, unsigned int height);
	bool Generate(unsigned int width, unsigned int height, unsigned int samples = 4);

	void Bind();
	void Blit();
	void UnBind();
	void BindTexture(unsigned int slot = 0);
	void UnBindTexture();
	void BindTextureMultiSample(unsigned int slot = 0);
	void UnBindTextureMS();

	unsigned int GetSampleCount() { return m_SampleCount; }

	void Delete();
	~MSAA();
private:
	unsigned int m_Width,
			     m_Height,
				 m_SampleCount;

	unsigned int m_ID,
				 m_RenderbufferID,
				 m_TextureID;

	unsigned int m_MultiSampleTex,
				 m_IntermidateFBO;
};