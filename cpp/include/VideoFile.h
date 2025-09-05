#pragma once

#include <string>
#include <vector>

#ifdef USE_EXPERIMENTAL_FILESYSTEM
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

/**
 * Represents a video file with metadata
 */
class VideoFile {
public:
    std::string name;
    std::string path;
    double sizeMB;
    std::string extension;

    // Constructor
    VideoFile(const std::string& name, const std::string& path, double sizeMB, const std::string& extension);

    // Static factory method to create VideoFile from filesystem path
    static VideoFile fromPath(const fs::path& filePath);

    // Check if file has supported video extension
    static bool isSupportedVideo(const std::string& extension);

    // Get list of supported extensions
    static std::vector<std::string> getSupportedExtensions();

    // String representation
    std::string toString() const;
};
