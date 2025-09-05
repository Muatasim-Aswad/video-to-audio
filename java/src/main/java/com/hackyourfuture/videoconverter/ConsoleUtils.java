package com.hackyourfuture.videoconverter;

import java.util.List;
import java.util.Scanner;

/**
 * Utility class for enhanced console output and user interaction.
 * Pure Java implementation without external dependencies.
 */
public class ConsoleUtils {
    
    private static final Scanner scanner = new Scanner(System.in);
    
    // ANSI color codes
    public static final String RESET = "\u001B[0m";
    public static final String RED = "\u001B[31m";
    public static final String GREEN = "\u001B[32m";
    public static final String YELLOW = "\u001B[33m";
    public static final String BLUE = "\u001B[34m";
    public static final String CYAN = "\u001B[36m";
    public static final String WHITE = "\u001B[37m";
    public static final String BOLD = "\u001B[1m";
    
    /**
     * Print colored text to console
     */
    public static void printColored(String text, String color) {
        System.out.print(color + text + RESET);
    }
    
    /**
     * Print colored line to console
     */
    public static void printlnColored(String text, String color) {
        System.out.println(color + text + RESET);
    }
    
    /**
     * Print success message (green)
     */
    public static void printSuccess(String message) {
        printlnColored("✅ " + message, GREEN);
    }
    
    /**
     * Print error message (red)
     */
    public static void printError(String message) {
        printlnColored("❌ " + message, RED);
    }
    
    /**
     * Print info message (blue)
     */
    public static void printInfo(String message) {
        printlnColored(message, BLUE);
    }
    
    /**
     * Print warning message (yellow)
     */
    public static void printWarning(String message) {
        printlnColored("⚠️ " + message, YELLOW);
    }
    
    /**
     * Print a simple ASCII header
     */
    public static void printHeader() {
        printlnColored("╔══════════════════════════════════════════╗", CYAN);
        printlnColored("║          Video to Audio Converter       ║", CYAN);
        printlnColored("║                 (Java)                   ║", CYAN);
        printlnColored("╚══════════════════════════════════════════╝", CYAN);
        System.out.println();
    }
    
    /**
     * Ask user for yes/no confirmation
     */
    public static boolean askConfirmation(String question, boolean defaultValue) {
        String defaultText = defaultValue ? "[Y/n]" : "[y/N]";
        printColored(question + " " + defaultText + ": ", YELLOW);
        
        String input = scanner.nextLine().trim().toLowerCase();
        
        if (input.isEmpty()) {
            return defaultValue;
        }
        
        return input.equals("y") || input.equals("yes");
    }
    
    /**
     * Ask user for text input
     */
    public static String askInput(String question) {
        printColored(question + ": ", YELLOW);
        return scanner.nextLine().trim();
    }
    
    /**
     * Display a numbered selection menu and get user choice
     */
    public static int selectFromMenu(String title, List<String> options) {
        System.out.println();
        printlnColored(title, CYAN);
        printlnColored("═".repeat(title.length()), CYAN);
        
        for (int i = 0; i < options.size(); i++) {
            System.out.printf("%s%s%d%s. %s%n", 
                GREEN + BOLD, 
                "", i + 1,
                RESET,
                options.get(i));
        }
        
        while (true) {
            printColored("\nSelect an option (1-" + options.size() + "): ", YELLOW);
            try {
                String input = scanner.nextLine().trim();
                int choice = Integer.parseInt(input);
                
                if (choice >= 1 && choice <= options.size()) {
                    return choice - 1; // Return zero-based index
                } else {
                    printError("Please enter a number between 1 and " + options.size());
                }
            } catch (NumberFormatException e) {
                printError("Please enter a valid number");
            }
        }
    }
    
    /**
     * Print a progress indicator
     */
    public static void printProgress(String message) {
        printColored("🔄 " + message, BLUE);
        System.out.flush();
    }
    
    /**
     * Clear the current line (for updating progress)
     */
    public static void clearLine() {
        System.out.print("\r" + " ".repeat(80) + "\r");
        System.out.flush();
    }
    
    /**
     * Print file size information
     */
    public static void printFileSize(String label, double sizeMB) {
        printlnColored(String.format("📊 %s: %.2f MB", label, sizeMB), YELLOW);
    }
    
    /**
     * Cleanup resources (no-op for pure Java version)
     */
    public static void cleanup() {
        // No cleanup needed for pure Java version
    }
}
