#pragma once

//#include "RendererErrorAssertion.h"
#include <string>


enum TextureType
{
	TextureType_NONE,
	TextureType_DIFFUSE,
	TextureType_SPECULAR,
	TextureType_EMISSIVE,
};


//std::string TypeToString(TextureType type)
//{
//	switch (type)
//	{
//	case TextureType_NONE:
//		return "None";
//		break;
//	case TextureType_DIFFUSE:
//		return "texture_diffuse";
//		break;
//	case TextureType_SPECULAR:
//		return "texture_specular";
//		break;
//	case TextureType_EMISSIVE:
//		return "texture_emissive";
//		break;
//	default:
//		return "None";
//		break;
//	}
//}



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

	std::string relativePath = "";
	TextureType m_type;

public:
	Texture();
	Texture(const std::string& fileLoc);
	~Texture();

	void RegisterUse();
	void UnRegisterUse();
	inline int const RefCount() const { return m_RefCount; }

	bool LoadTexture(const std::string& fileLoc, TextureType type = TextureType_NONE);
	void Activate(unsigned int slot = 0) const;
	void DisActivate() const;
	void Clear();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

	void RelativePath(std::string r_path) { relativePath = r_path; }
	inline std::string GetRelativePath() { return relativePath; }

	inline std::string GetFilePath() { return m_FileLocation; }
	inline TextureType GetType() { return m_type; }
};

