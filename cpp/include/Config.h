#pragma once

#include <string>
#include <unordered_map>

/**
 * Configuration settings for the video converter
 */
class Config {
private:
    std::string defaultDir;
    
    // Load environment variables from .env file
    std::unordered_map<std::string, std::string> loadEnvFile(const std::string& filePath);

public:
    // Constructor - loads configuration from .env file
    Config();

    // Getters
    const std::string& getDefaultDir() const { return defaultDir; }

    // Setters
    void setDefaultDir(const std::string& dir) { defaultDir = dir; }
};
