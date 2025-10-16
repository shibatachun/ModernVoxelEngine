#pragma once



#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/version.h>
#include <assimp/Importer.hpp>
#include <assimp/GltfMaterial.h>

//tinygltf loader
#include "../utils/tiny_gltf.h"
#include "../utils/TypeDefine.h"
#include "../utils/minilog.h"
#include "../utils/Utils.h"
#include "DataFormat.h"



namespace asset
{
	inline static std::vector<char> readFile(const std::filesystem::path& filepath)
	{
#if defined(_WIN32)
		std::ifstream file(filepath.wstring(), std::ios::ate | std::ios::binary);
#else
		std::ifstream file(filepath.u8string(), std::ios::ate | std::ios::binary);
#endif

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
    inline int GetFileSize(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file) {
            return -1;
        }
        return static_cast<int>(file.tellg());
    }

    inline bool FileExists(const std::string_view name) {
        std::error_code ec;
        return std::filesystem::exists(std::filesystem::path(name), ec);
    }

    inline bool RenameFile(const std::string& oldFilePath, const std::string& newFilePath) {
        if (!FileExists(oldFilePath)) {
            std::cout << "Util::RenameFile() failed because old path '" << oldFilePath << "' does not exist!\n";
            return false;
        }
        std::filesystem::path oldPath = oldFilePath;
        std::filesystem::path newPath = newFilePath;
        return std::filesystem::rename(oldPath, newPath), true;
    }

    inline std::string GetFileName(const std::string& filepath) {
        size_t pos = filepath.find_last_of("/\\");
        std::string filename = (pos == std::string::npos) ? filepath : filepath.substr(pos + 1);
        pos = filename.find_last_of('.');
        return (pos != std::string::npos) ? filename.substr(0, pos) : filename;
    }

    inline std::string RemoveFileExtension(const std::string& filename) {
        size_t pos = filename.find_last_of('.');
        return (pos != std::string::npos) ? filename.substr(0, pos) : filename;
    }

    inline std::string GetFullPath(const std::filesystem::directory_entry& entry) {
        return entry.path().string();
    }

    inline std::string GetFileName(const std::filesystem::directory_entry& entry) {
        return entry.path().filename().string();
    }

    inline std::string GetFileNameWithoutExtension(const std::filesystem::directory_entry& entry) {
        return entry.path().stem().string();
    }

    inline std::string GetFileExtension(const std::filesystem::directory_entry& entry) {
        return entry.path().extension().string().substr(1);
    }

    inline std::vector<FileInfo> IterateDirectory(const std::string& directory, std::vector<std::string> extensions) {
        std::vector<FileInfo> fileInfoList;
        if (!std::filesystem::exists(directory)) return fileInfoList;

        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (!std::filesystem::is_regular_file(entry)) continue;

            FileInfo fileInfo = { GetFullPath(entry), GetFileNameWithoutExtension(entry), GetFileExtension(entry), directory };

            if (extensions.empty() || std::find(extensions.begin(), extensions.end(), fileInfo.ext) != extensions.end()) {
                fileInfoList.push_back(fileInfo);
            }
        }
        return fileInfoList;
    }
    inline void IterateAllDirectories(const std::string& directory, std::vector<std::string> extensions, std::vector<FileInfo>& fileinfos) {

        if (!std::filesystem::exists(directory)) throw std::runtime_error("Not a validate directory");

        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (!std::filesystem::is_regular_file(entry)) {
                if (std::filesystem::is_directory(entry)) {
                    IterateAllDirectories(entry.path().string(), extensions, fileinfos);
                    continue;
                }
                else
                {
                    continue;
                }
            }

            FileInfo fileInfo = { GetFullPath(entry), GetFileNameWithoutExtension(entry), GetFileExtension(entry), directory };

            if (extensions.empty() || std::find(extensions.begin(), extensions.end(), fileInfo.ext) != extensions.end()) {
                fileinfos.push_back(fileInfo);
            }
            

        }
    }

    inline std::string GetFilename(const std::string& filepath) {
        std::string result = filepath.substr(filepath.rfind("/") + 1);
        return result.substr(0, result.length() - 4);
    }

    inline FileInfo GetFileInfoFromPath(const std::string& filepath) {
        FileInfo fileInfo;
        std::filesystem::path path(filepath);
        if (!FileExists(filepath)) {
            std::cout << "Util::GetFileInfoFromPath() Failed: path does not exist: " << filepath << ", warning returning an empty FileInfo\n";
            return fileInfo;
        }
        fileInfo.path = path.string();
        fileInfo.name = path.stem().string();
        fileInfo.ext = path.has_extension() ? path.extension().string().substr(1) : "";
        fileInfo.dir = path.parent_path().string();
        return fileInfo;
    }

    inline uint64_t GetLastModifiedTime(const std::string& filePath)
    {
        try {
            auto ftime = std::filesystem::last_write_time(filePath);
            auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
            return std::chrono::duration_cast<std::chrono::seconds>(systemTime.time_since_epoch()).count();
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error reading file timestamp: " << e.what() << "\n";
            return 0;
        }
    }
}
