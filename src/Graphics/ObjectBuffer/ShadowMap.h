#pragma once



class ShadowMap
{
public:
	ShadowMap() = default;
	~ShadowMap() = default;
	ShadowMap(unsigned int width, unsigned int height) : m_Width(width), m_Height(height){}
	void Generate(unsigned int width, unsigned int height);
	virtual void Generate();
	void Write();
	virtual void Read(unsigned int slot = 0);
	void BindMapTexture(unsigned int slot = 0);
	virtual void UnBindMap();
	void UnBind();
protected:
	unsigned int m_Id = 0;
	unsigned int m_Height = 2048;// 1024;// 4096;//2048;
	unsigned int m_Width = 2048;//1024;// 4096;// 2048;


	unsigned int m_TexMapId = 0;
};



//---------------------------------Shadow Cube---------------------------------/
class ShadowCube : public ShadowMap
{
public:
	ShadowCube() = default;
	~ShadowCube() = default;
	ShadowCube(unsigned int width, unsigned int height) : ShadowMap(width, height) {}
	virtual void Generate() override;
	virtual void Read(unsigned int slot = 0) override;
	virtual void UnBindMap() override;

};


