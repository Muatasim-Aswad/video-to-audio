package com.hackyourfuture.videoconverter;

/**
 * Main application entry point for the Video to Audio Converter.
 * 
 * This Java application provides interactive video to audio conversion
 * with file selection menus and progress display.
 * 
 * Features:
 * - Interactive directory and file selection
 * - Support for multiple video formats
 * - Progress display during conversion
 * - Configuration via .env file
 * - Color-coded console output
 * 
 * @author HackYourFuture
 * @version 1.0.0
 */
public class VideoConverterApp {
    
    public static void main(String[] args) {
        // Set up shutdown hook for cleanup
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            ConsoleUtils.cleanup();
        }));
        
        try {
            VideoConverter converter = new VideoConverter();
            converter.run();
        } catch (Exception e) {
            ConsoleUtils.printError("Fatal error: " + e.getMessage());
            System.exit(1);
        } finally {
            ConsoleUtils.cleanup();
        }
    }
}
