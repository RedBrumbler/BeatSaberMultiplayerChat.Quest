#include "Utilities/FileUtils.hpp"
#include "logging.hpp"
#include <dirent.h>

namespace MultiplayerChat::Utilities {
    std::string_view FileUtils::GetFileName(std::string_view path, bool removeExtension) {
        int pos = path.find_last_of("/");
        if (pos != std::string::npos)
            return removeExtension ? RemoveExtension(path.substr(pos + 1)) : path.substr(pos + 1);
        else
            return removeExtension ? RemoveExtension(path) : path;
    }

    std::string_view FileUtils::RemoveExtension(std::string_view path) {
        int pos = path.find_last_of(".");
        if (pos != std::string::npos)
            return path.substr(0, pos);
        return path;
    }

    std::string_view FileUtils::GetExtension(std::string_view path) {
        int pos = path.find_last_of(".");
        if (pos != std::string::npos)
            return path.substr(pos + 1);
        return "";
    }

    bool FileUtils::GetFilesInFolderPath(std::string_view extension, std::string_view filePath, std::vector<std::string>& out) {
        bool foundTheExtension = false;
        struct DIR* fileDir = opendir(filePath.data());
        struct dirent* files;
        if (fileDir != NULL) {
            while ((files = readdir(fileDir)) != NULL) {
                char* fileName = files->d_name;
                INFO("Found file {}", fileName);
                if (!strcmp(fileName, ".") || !strcmp(fileName, ".."))
                    continue;
                if (GetExtension(fileName) == extension) {
                    out.push_back(fileName);
                    foundTheExtension = true;
                }
            }
            closedir(fileDir);
        }
        return foundTheExtension;
    }
}
