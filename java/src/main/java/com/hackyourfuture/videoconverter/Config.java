package com.hackyourfuture.videoconverter;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;

/**
 * Configuration settings for the video converter application.
 */
public class Config {
    private String defaultDir = "/Users/hackyourfuture/Downloads";
    
    public Config() {
        loadFromEnvFile();
    }
    
    /**
     * Load configuration from .env file
     */
    private void loadFromEnvFile() {
        try {
            // Try to load .env file from parent directory
            Path envPath = Paths.get("..", ".env");
            
            if (envPath.toFile().exists()) {
                Map<String, String> envVars = parseEnvFile(envPath.toString());
                
                String defaultDirFromEnv = envVars.get("DEFAULT_DIR");
                if (defaultDirFromEnv != null && !defaultDirFromEnv.trim().isEmpty()) {
                    this.defaultDir = defaultDirFromEnv.trim();
                }
            }
        } catch (Exception e) {
            // Ignore errors and use default
            System.err.println("Warning: Could not load .env file, using defaults");
        }
    }
    
    /**
     * Parse .env file and return key-value pairs
     */
    private Map<String, String> parseEnvFile(String filePath) throws IOException {
        Map<String, String> envVars = new HashMap<>();
        
        try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) {
            String line;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty() || line.startsWith("#") || !line.contains("=")) {
                    continue;
                }
                
                String[] parts = line.split("=", 2);
                if (parts.length == 2) {
                    envVars.put(parts[0].trim(), parts[1].trim());
                }
            }
        }
        
        return envVars;
    }
    
    public String getDefaultDir() {
        return defaultDir;
    }
    
    public void setDefaultDir(String defaultDir) {
        this.defaultDir = defaultDir;
    }
}
