#pragma once

#include <string>
#include <vector>

/**
 * Utility class for enhanced console output and user interaction
 */
class ConsoleUtils {
public:
    // ANSI color codes
    static const std::string RESET;
    static const std::string RED;
    static const std::string GREEN;
    static const std::string YELLOW;
    static const std::string BLUE;
    static const std::string CYAN;
    static const std::string WHITE;
    static const std::string BOLD;

    // Print colored text
    static void printColored(const std::string& text, const std::string& color);
    static void printlnColored(const std::string& text, const std::string& color);

    // Convenience methods for common message types
    static void printSuccess(const std::string& message);
    static void printError(const std::string& message);
    static void printInfo(const std::string& message);
    static void printWarning(const std::string& message);

    // Print application header
    static void printHeader();

    // User interaction
    static bool askConfirmation(const std::string& question, bool defaultValue = true);
    static std::string askInput(const std::string& question);
    static int selectFromMenu(const std::string& title, const std::vector<std::string>& options);

    // Progress and status
    static void printProgress(const std::string& message);
    static void clearLine();
    static void printFileSize(const std::string& label, double sizeMB);

private:
    // Helper methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
