#pragma once
#include <string>

class FileSystemUtils {
public:
    static std::string getExecutablePath();
    static std::string getMaterialFilePath(const std::string& materialFileName);

    // New method to get the path for any asset relative to the executable path
    static std::string getAssetFilePath(const std::string& assetRelativePath) {
        std::string exePath = getExecutablePath();
        // Assuming all assets are located under a 'media' directory
        return exePath + "/media/" + assetRelativePath;
    }
};
