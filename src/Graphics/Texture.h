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

enum class TextureFormat
{
	RGB = 3,  //for now because of model loading
	RGBA,
	SRGB,
	SRGBA
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
	unsigned int m_Id = 0;
	std::string m_FileLocation = "";
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BitDepth;
public:
	Texture();
	Texture(const std::string& fileLoc, TextureFormat format = TextureFormat::RGBA, TextureType type = TextureType_NONE);
	~Texture();

	void RegisterUse();
	void UnRegisterUse();
	inline int const RefCount() const { return m_RefCount; }

	bool LoadTexture(const std::string& fileLoc, TextureFormat format = TextureFormat::RGBA, TextureType type = TextureType_NONE);
	void Activate(unsigned int slot = 0) const;
	void DisActivate() const;
	void Clear();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

	void RelativePath(std::string r_path) { relativePath = r_path; }
	inline std::string GetRelativePath() { return relativePath; }

	inline std::string GetFilePath() { return m_FileLocation; }
	inline TextureType GetType() { return m_TexType; }
	inline TextureFormat GetFormat() { return m_TexFormat; }
private:
	int m_RefCount = 0;
	std::string relativePath = "";
	TextureFormat m_TexFormat = TextureFormat::RGBA;
	TextureType m_TexType = TextureType_NONE;
};

