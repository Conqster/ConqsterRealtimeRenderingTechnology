#pragma once
#include <unordered_map>
#include <string>

#include <filesystem>

class FilePaths
{
public:
	FilePaths(const FilePaths&) = delete;

	static inline FilePaths& Instance()
	{
		static FilePaths instance;
		return instance;
	}



	bool RegisterPath(const std::string& key, const std::string& path)
	{
		if (!std::filesystem::exists(path))
		{
			printf("[FILE PATHS]: Failed to registered path '%s' does not exist!!!.\n", path.c_str());
			return false;
		}
		mPaths[key] = path;
		printf("[FILE PATHS]: Registered path '%s'.\n", path.c_str());
		return true;
	}

	inline std::string GetPath(const std::string& key)
	{
		auto it = mPaths.find(key);
		if (it != mPaths.end())
			return it->second;


		printf("[FILE PATHS]: key '%s' does not have a registered path!!!!!!\n", key.c_str());
		return "";
	}

private:
	FilePaths() = default;
	std::unordered_map<std::string, std::string> mPaths;
	std::string textureFolderDir = "Assets/Textures";     //use later, to store less string characters and concat on retrive 
};

