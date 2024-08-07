#pragma once

//#include "RendererErrorAssertion.h"
#include <string>


/// <summary>
/// TO-DO: Probaly later have something called a shared texture 
///		   It might be similar to batch rendering i think
///		   so similar texture wouldnt be duplicated in memory
/// </summary>
class Texture
{
private:
	unsigned int m_Id;
	std::string m_FileLocation;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BitDepth;

	int m_RefCount;

public:
	Texture();
	Texture(const std::string& fileLoc);
	~Texture();

	void RegisterUse();
	void UnRegisterUse();
	inline int const RefCount() const { return m_RefCount; }

	bool LoadTexture(const std::string& fileLoc);
	void Activate(unsigned int slot = 0) const;
	void DisActivate() const;
	void Clear();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};

