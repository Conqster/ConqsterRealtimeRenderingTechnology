#pragma once



class ShadowMap
{
public:
	void Generate();
	void Write();
	void Read(unsigned int slot = 0);
	void BindMapTexture(unsigned int slot = 0);
	void UnBind();
private:
	unsigned int m_Id;
	unsigned int m_Height = 2048;// 1024;// 4096;//2048;
	unsigned int m_Width = 2048;//1024;// 4096;// 2048;


	unsigned int m_TexMapId;
};