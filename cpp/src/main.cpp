#include "VideoConverter.h"
#include "ConsoleUtils.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

/**
 * Signal handler for graceful shutdown
 */
void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << std::endl;
        ConsoleUtils::printInfo("👋 Goodbye!");
        std::exit(0);
    }
}

/**
 * Main entry point for the C++ Video to Audio Converter
 * 
 * This application provides interactive video to audio conversion
 * with file selection menus and progress display.
 * 
 * Features:
 * - Interactive directory and file selection
 * - Support for multiple video formats
 * - Progress display during conversion
 * - Configuration via .env file
 * - Color-coded console output
 * 
 * @return Exit code (0 for success, non-zero for error)
 */
int main() {
    // Set up signal handler for graceful shutdown
    std::signal(SIGINT, signalHandler);
    
    try {
        VideoConverter converter;
        return converter.run();
    } catch (const std::exception& e) {
        ConsoleUtils::printError("Fatal error: " + std::string(e.what()));
        return 1;
    } catch (...) {
        ConsoleUtils::printError("Unknown fatal error occurred");
        return 1;
    }
}
