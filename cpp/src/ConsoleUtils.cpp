#include "ConsoleUtils.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <limits>
#include <iomanip>
#include <sstream>

// ANSI color codes
const std::string ConsoleUtils::RESET = "\033[0m";
const std::string ConsoleUtils::RED = "\033[31m";
const std::string ConsoleUtils::GREEN = "\033[32m";
const std::string ConsoleUtils::YELLOW = "\033[33m";
const std::string ConsoleUtils::BLUE = "\033[34m";
const std::string ConsoleUtils::CYAN = "\033[36m";
const std::string ConsoleUtils::WHITE = "\033[37m";
const std::string ConsoleUtils::BOLD = "\033[1m";

void ConsoleUtils::printColored(const std::string& text, const std::string& color) {
    std::cout << color << text << RESET;
    std::cout.flush();
}

void ConsoleUtils::printlnColored(const std::string& text, const std::string& color) {
    std::cout << color << text << RESET << std::endl;
}

void ConsoleUtils::printSuccess(const std::string& message) {
    printlnColored("✅ " + message, GREEN);
}

void ConsoleUtils::printError(const std::string& message) {
    printlnColored("❌ " + message, RED);
}

void ConsoleUtils::printInfo(const std::string& message) {
    printlnColored(message, BLUE);
}

void ConsoleUtils::printWarning(const std::string& message) {
    printlnColored("⚠️ " + message, YELLOW);
}

void ConsoleUtils::printHeader() {
    printlnColored("╔══════════════════════════════════════════╗", CYAN);
    printlnColored("║          Video to Audio Converter       ║", CYAN);
    printlnColored("║                 (C++)                    ║", CYAN);
    printlnColored("╚══════════════════════════════════════════╝", CYAN);
    std::cout << std::endl;
}

bool ConsoleUtils::askConfirmation(const std::string& question, bool defaultValue) {
    std::string defaultText = defaultValue ? "[Y/n]" : "[y/N]";
    printColored(question + " " + defaultText + ": ", YELLOW);
    
    std::string input;
    std::getline(std::cin, input);
    input = trim(toLower(input));
    
    if (input.empty()) {
        return defaultValue;
    }
    
    return input == "y" || input == "yes";
}

std::string ConsoleUtils::askInput(const std::string& question) {
    printColored(question + ": ", YELLOW);
    std::string input;
    std::getline(std::cin, input);
    return trim(input);
}

int ConsoleUtils::selectFromMenu(const std::string& title, const std::vector<std::string>& options) {
    std::cout << std::endl;
    printlnColored(title, CYAN);
    printlnColored(std::string(title.length(), '='), CYAN);
    
    for (size_t i = 0; i < options.size(); ++i) {
        printColored(std::to_string(i + 1) + ". ", GREEN + BOLD);
        std::cout << options[i] << std::endl;
    }
    
    while (true) {
        printColored("\nSelect an option (1-" + std::to_string(options.size()) + "): ", YELLOW);
        
        std::string input;
        std::getline(std::cin, input);
        input = trim(input);
        
        try {
            int choice = std::stoi(input);
            if (choice >= 1 && choice <= static_cast<int>(options.size())) {
                return choice - 1; // Return zero-based index
            } else {
                printError("Please enter a number between 1 and " + std::to_string(options.size()));
            }
        } catch (const std::exception&) {
            printError("Please enter a valid number");
        }
    }
}

void ConsoleUtils::printProgress(const std::string& message) {
    printColored("🔄 " + message, BLUE);
    std::cout.flush();
}

void ConsoleUtils::clearLine() {
    std::cout << "\r" << std::string(80, ' ') << "\r";
    std::cout.flush();
}

void ConsoleUtils::printFileSize(const std::string& label, double sizeMB) {
    std::ostringstream oss;
    oss << "📊 " << label << ": " << std::fixed << std::setprecision(2) << sizeMB << " MB";
    printlnColored(oss.str(), YELLOW);
}

std::string ConsoleUtils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string ConsoleUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
