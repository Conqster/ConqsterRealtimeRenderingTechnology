#pragma once



class ShadowMap
{
public:
	ShadowMap() = default;
	~ShadowMap() = default;
	virtual void Generate();
	void Write();
	virtual void Read(unsigned int slot = 0);
	void BindMapTexture(unsigned int slot = 0);
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

	virtual void Generate() override;
	virtual void Read(unsigned int slot = 0) override;

};


