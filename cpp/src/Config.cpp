#include "Config.h"
#include <fstream>
#include <iostream>

#ifdef USE_EXPERIMENTAL_FILESYSTEM
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

Config::Config() : defaultDir("/Users/hackyourfuture/Downloads") {
    // Try to load .env file from parent directory
    fs::path envPath = fs::path("..") / ".env";
    
    if (fs::exists(envPath)) {
        try {
            auto envVars = loadEnvFile(envPath.string());
            auto it = envVars.find("DEFAULT_DIR");
            if (it != envVars.end() && !it->second.empty()) {
                defaultDir = it->second;
            }
        } catch (const std::exception& e) {
            // Ignore errors and use default
            std::cerr << "Warning: Could not load .env file, using defaults" << std::endl;
        }
    }
}

std::unordered_map<std::string, std::string> Config::loadEnvFile(const std::string& filePath) {
    std::unordered_map<std::string, std::string> envVars;
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open .env file");
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Find equals sign
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Trim key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (!key.empty()) {
            envVars[key] = value;
        }
    }
    
    return envVars;
}
