#pragma once

#include "Config.h"
#include "VideoFile.h"
#include <vector>
#include <string>
#include <memory>

/**
 * Main video converter class with interactive functionality
 */
class VideoConverter {
private:
    std::unique_ptr<Config> config;

    // Helper methods
    bool isUrl(const std::string& input) const;
    std::vector<VideoFile> getVideoFiles(const std::string& directory) const;
    std::string getOutputPath(const std::string& inputPath) const;
    bool convertToMp3(const std::string& inputPath, const std::string& outputPath) const;
    double getFileSizeMB(const std::string& filePath) const;

    // User interaction methods
    std::string askDirectory() const;
    std::string selectVideoFile(const std::vector<VideoFile>& videoFiles, const std::string& rootDir) const;

public:
    // Constructor
    VideoConverter();

    // Destructor
    ~VideoConverter() = default;

    // Main application logic
    int run();
};
