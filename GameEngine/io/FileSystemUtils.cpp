#include "FileSystemUtils.h"
#include <string>
#include <algorithm>
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
// Windows specific includes
#include <windows.h>
#elif defined(__linux__)
// Linux specific includes
#include <unistd.h>
#include <limits.h>
#endif

std::string wcharToString(const wchar_t* wcharArray) {
    if (wcharArray == nullptr) return std::string();

    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wcharArray, -1, nullptr, 0, nullptr, nullptr);
    std::string strTo(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, wcharArray, -1, &strTo[0], sizeNeeded, nullptr, nullptr);

    // The string constructor adds a null terminator, remove it
    if (!strTo.empty() && strTo.back() == '\0') {
        strTo.pop_back();
    }

    return strTo;
}

std::string FileSystemUtils::getExecutablePath() {
    std::string fullPath;

#if defined(_WIN32) || defined(_WIN64)
    // Windows implementation
    wchar_t path[MAX_PATH] = { 0 };
    HMODULE hModule = GetModuleHandleW(NULL); // Explicitly use the wide-char version
    if (hModule != NULL) {
        GetModuleFileNameW(hModule, path, MAX_PATH); // Using the Unicode version
        fullPath = wcharToString(path);
    }
#elif defined(__linux__)
    // Linux implementation
    char result[PATH_MAX] = { 0 };
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        fullPath = std::string(result, (count > 0) ? count : 0);
    }
#endif

    // Convert backslashes to forward slashes for cross-platform compatibility
    std::replace(fullPath.begin(), fullPath.end(), '\\', '/');
    std::string::size_type pos = fullPath.find_last_of('/');
    if (pos != std::string::npos) {
        fullPath = fullPath.substr(0, pos);
    }

    return fullPath;
}

std::string FileSystemUtils::getMaterialFilePath(const std::string& materialFileName) {
    std::string exePath = getExecutablePath();
    std::string materialPath = exePath + "/media/materials/" + materialFileName;
    std::cout << "Constructed Material File Path: " << materialPath << std::endl;
    return materialPath;
}