#pragma once



class ShadowMap
{
public:
	ShadowMap() = default;
	~ShadowMap() = default;
	ShadowMap(unsigned int res) : m_Width(res), m_Height(res){}
	ShadowMap(unsigned int width, unsigned int height) : m_Width(width), m_Height(height){}
	virtual void Generate(unsigned int res);
	virtual void Generate(unsigned int width, unsigned int height);
	virtual void Generate();
	void Write();
	virtual void Read(unsigned int slot = 0);
	void BindMapTexture(unsigned int slot = 0);
	virtual void UnBindMap();
	void UnBind();
	void Clear();

	inline unsigned int GetSize() { return m_Width; }
	inline unsigned int GetColourAttactment() { return m_TexMapId; }
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
	ShadowCube(unsigned int res) : ShadowMap(res) {}
	ShadowCube(unsigned int width, unsigned int height) : ShadowMap(width, height) {}
	void Generate(unsigned int width, unsigned int height) override;
	virtual void Generate() override;
	virtual void Read(unsigned int slot = 0) override;
	virtual void UnBindMap() override;

};


