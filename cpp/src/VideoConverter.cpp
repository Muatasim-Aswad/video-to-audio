#include "VideoConverter.h"
#include "ConsoleUtils.h"
#include <algorithm>
#include <iostream>
#include <regex>
#include <cstdlib>
#include <sstream>

#ifdef USE_EXPERIMENTAL_FILESYSTEM
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

VideoConverter::VideoConverter() : config(std::make_unique<Config>()) {}

bool VideoConverter::isUrl(const std::string& input) const {
    // Simple URL validation for HTTP/HTTPS
    std::regex urlRegex(R"(^https?://.*)", std::regex_constants::icase);
    return std::regex_match(input, urlRegex);
}

std::vector<VideoFile> VideoConverter::getVideoFiles(const std::string& directory) const {
    std::vector<VideoFile> videoFiles;
    
    try {
        fs::path dirPath(directory);
        if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
            return videoFiles;
        }
        
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                if (!extension.empty() && extension[0] == '.') {
                    extension = extension.substr(1);
                }
                
                if (VideoFile::isSupportedVideo(extension)) {
                    try {
                        videoFiles.push_back(VideoFile::fromPath(entry.path()));
                    } catch (const std::exception& e) {
                        // Skip files we can't read
                        continue;
                    }
                }
            }
        }
        
        // Sort by filename (case-insensitive)
        std::sort(videoFiles.begin(), videoFiles.end(), [](const VideoFile& a, const VideoFile& b) {
            std::string aName = a.name;
            std::string bName = b.name;
            std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
            std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);
            return aName < bName;
        });
        
    } catch (const std::exception& e) {
        ConsoleUtils::printError("Error reading directory: " + std::string(e.what()));
    }
    
    return videoFiles;
}

std::string VideoConverter::getOutputPath(const std::string& inputPath) const {
    if (isUrl(inputPath)) {
        // For URLs, extract filename from path
        size_t lastSlash = inputPath.find_last_of('/');
        std::string filename = (lastSlash != std::string::npos) ? inputPath.substr(lastSlash + 1) : "output";
        
        // Remove extension if present
        size_t lastDot = filename.find_last_of('.');
        if (lastDot != std::string::npos) {
            filename = filename.substr(0, lastDot);
        }
        
        return filename + ".mp3";
    }
    
    fs::path inputFilePath(inputPath);
    fs::path outputPath = inputFilePath.parent_path() / (inputFilePath.stem().string() + ".mp3");
    return outputPath.string();
}

bool VideoConverter::convertToMp3(const std::string& inputPath, const std::string& outputPath) const {
    ConsoleUtils::printProgress("Converting: " + fs::path(inputPath).filename().string() + 
                               " → " + fs::path(outputPath).filename().string());
    std::cout << std::endl;
    
    // Build ffmpeg command
    std::ostringstream cmdStream;
    cmdStream << "ffmpeg -i \"" << inputPath << "\" -vn -acodec libmp3lame -ab 192k -y \"" << outputPath << "\" 2>&1";
    std::string command = cmdStream.str();
    
    ConsoleUtils::printInfo("🎬 FFmpeg started...");
    
    // Execute command
    int result = std::system(command.c_str());
    
    ConsoleUtils::clearLine();
    
    if (result == 0) {
        ConsoleUtils::printSuccess("Conversion completed successfully!");
        return true;
    } else {
        if (result == 32512 || result == 127) { // Command not found
            ConsoleUtils::printError("FFmpeg not found. Please make sure FFmpeg is installed and in your PATH.");
        } else {
            ConsoleUtils::printError("FFmpeg failed with exit code: " + std::to_string(result));
        }
        return false;
    }
}

double VideoConverter::getFileSizeMB(const std::string& filePath) const {
    try {
        std::error_code ec;
        auto fileSize = fs::file_size(filePath, ec);
        if (ec) {
            return 0.0;
        }
        return static_cast<double>(fileSize) / (1024.0 * 1024.0);
    } catch (const std::exception&) {
        return 0.0;
    }
}

std::string VideoConverter::askDirectory() const {
    ConsoleUtils::printInfo("📁 Default directory: " + config->getDefaultDir());
    
    if (ConsoleUtils::askConfirmation("Use this as the root directory?", true)) {
        return config->getDefaultDir();
    }
    
    while (true) {
        std::string customDir = ConsoleUtils::askInput("Enter root directory path");
        
        if (fs::exists(customDir) && fs::is_directory(customDir)) {
            return customDir;
        }
        
        ConsoleUtils::printError("Directory not found: " + customDir);
    }
}

std::string VideoConverter::selectVideoFile(const std::vector<VideoFile>& videoFiles, const std::string& rootDir) const {
    if (videoFiles.empty()) {
        ConsoleUtils::printError("No supported video files found in the directory.");
        ConsoleUtils::printInfo("💡 Supported formats: mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp");
        
        if (!ConsoleUtils::askConfirmation("Would you like to enter a file path or URL manually?", true)) {
            ConsoleUtils::printInfo("👋 Goodbye!");
            std::exit(0);
        }
        
        return ConsoleUtils::askInput("Enter video file path or URL");
    }
    
    ConsoleUtils::printInfo("📹 Found " + std::to_string(videoFiles.size()) + " video file(s)");
    
    std::vector<std::string> choices;
    for (const auto& vf : videoFiles) {
        choices.push_back(vf.toString());
    }
    choices.push_back("📝 Enter file path or URL manually");
    
    int selection = ConsoleUtils::selectFromMenu("Select a video file to convert:", choices);
    
    if (selection == static_cast<int>(choices.size()) - 1) {
        return ConsoleUtils::askInput("Enter video file path or URL");
    }
    
    return videoFiles[selection].name;
}

int VideoConverter::run() {
    ConsoleUtils::printHeader();
    
    try {
        // Get working directory
        std::string rootDir = askDirectory();
        ConsoleUtils::printInfo("🗂️  Working in: " + rootDir);
        
        // Get video files in directory
        auto videoFiles = getVideoFiles(rootDir);
        
        // Let user select file
        std::string selectedFile = selectVideoFile(videoFiles, rootDir);
        
        // Determine full input path
        std::string fullInputPath;
        if (isUrl(selectedFile) || fs::path(selectedFile).is_absolute()) {
            fullInputPath = selectedFile;
        } else {
            fullInputPath = (fs::path(rootDir) / selectedFile).string();
        }
        
        // Check if local file exists
        if (!isUrl(fullInputPath)) {
            if (!fs::exists(fullInputPath)) {
                ConsoleUtils::printError("File not found: " + fullInputPath);
                return 1;
            }
        }
        
        // Generate output path
        std::string outputPath = getOutputPath(fullInputPath);
        
        std::cout << std::endl;
        ConsoleUtils::printInfo("📁 Input: " + fullInputPath);
        ConsoleUtils::printInfo("💾 Output: " + outputPath);
        std::cout << std::endl;
        
        // Perform conversion
        if (convertToMp3(fullInputPath, outputPath)) {
            ConsoleUtils::printSuccess("Conversion finished: " + outputPath);
            
            // Show output file size
            double sizeMB = getFileSizeMB(outputPath);
            if (sizeMB > 0.0) {
                ConsoleUtils::printFileSize("Output file size", sizeMB);
            }
        } else {
            ConsoleUtils::printError("Conversion failed!");
            return 1;
        }
        
    } catch (const std::exception& e) {
        ConsoleUtils::printError("Unexpected error: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}
