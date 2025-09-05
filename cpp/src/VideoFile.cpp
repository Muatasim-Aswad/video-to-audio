#include "VideoFile.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

VideoFile::VideoFile(const std::string& name, const std::string& path, double sizeMB, const std::string& extension)
    : name(name), path(path), sizeMB(sizeMB), extension(extension) {}

VideoFile VideoFile::fromPath(const fs::path& filePath) {
    std::error_code ec;
    auto fileSize = fs::file_size(filePath, ec);
    
    if (ec) {
        throw std::runtime_error("Could not get file size for: " + filePath.string());
    }
    
    double sizeMB = static_cast<double>(fileSize) / (1024.0 * 1024.0);
    std::string name = filePath.filename().string();
    std::string extension = filePath.extension().string();
    
    // Remove the dot from extension and convert to lowercase
    if (!extension.empty() && extension[0] == '.') {
        extension = extension.substr(1);
    }
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return VideoFile(name, filePath.string(), sizeMB, extension);
}

bool VideoFile::isSupportedVideo(const std::string& extension) {
    auto supportedExtensions = getSupportedExtensions();
    std::string lowerExt = extension;
    std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
    
    return std::find(supportedExtensions.begin(), supportedExtensions.end(), lowerExt) != supportedExtensions.end();
}

std::vector<std::string> VideoFile::getSupportedExtensions() {
    return {"mp4", "avi", "mov", "mkv", "flv", "wmv", "webm", "m4v", "3gp"};
}

std::string VideoFile::toString() const {
    std::ostringstream oss;
    oss << name << " (" << std::fixed << std::setprecision(1) << sizeMB << " MB)";
    return oss.str();
}
