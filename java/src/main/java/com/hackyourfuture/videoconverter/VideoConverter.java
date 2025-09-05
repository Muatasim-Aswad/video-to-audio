package com.hackyourfuture.videoconverter;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.TimeUnit;

/**
 * Main video converter class with interactive functionality.
 */
public class VideoConverter {
    
    private static final List<String> SUPPORTED_EXTENSIONS = Arrays.asList(
        ".mp4", ".avi", ".mov", ".mkv", ".flv", ".wmv", ".webm", ".m4v", ".3gp"
    );
    
    private final Config config;
    
    public VideoConverter() {
        this.config = new Config();
    }
    
    /**
     * Check if a string is a valid URL
     */
    private boolean isUrl(String input) {
        try {
            URL url = new URL(input);
            return url.getProtocol().equals("http") || url.getProtocol().equals("https");
        } catch (MalformedURLException e) {
            return false;
        }
    }
    
    /**
     * Get all video files in a directory
     */
    private List<VideoFile> getVideoFiles(String directory) {
        List<VideoFile> videoFiles = new ArrayList<>();
        
        try {
            File dir = new File(directory);
            if (!dir.exists() || !dir.isDirectory()) {
                return videoFiles;
            }
            
            File[] files = dir.listFiles();
            if (files == null) {
                return videoFiles;
            }
            
            for (File file : files) {
                if (file.isFile()) {
                    String extension = getFileExtension(file.getName()).toLowerCase();
                    if (SUPPORTED_EXTENSIONS.contains(extension)) {
                        videoFiles.add(VideoFile.fromFile(file));
                    }
                }
            }
            
            // Sort by filename (case-insensitive)
            videoFiles.sort((a, b) -> a.name().compareToIgnoreCase(b.name()));
            
        } catch (Exception e) {
            ConsoleUtils.printError("Error reading directory: " + e.getMessage());
        }
        
        return videoFiles;
    }
    
    /**
     * Get file extension from filename
     */
    private String getFileExtension(String filename) {
        int dotIndex = filename.lastIndexOf('.');
        return (dotIndex > 0) ? filename.substring(dotIndex) : "";
    }
    
    /**
     * Generate output MP3 path based on input path
     */
    private String getOutputPath(String inputPath) {
        if (isUrl(inputPath)) {
            try {
                URL url = new URL(inputPath);
                String path = url.getPath();
                String baseName = Paths.get(path).getFileName().toString();
                int dotIndex = baseName.lastIndexOf('.');
                if (dotIndex > 0) {
                    baseName = baseName.substring(0, dotIndex);
                }
                return baseName.isEmpty() ? "output.mp3" : baseName + ".mp3";
            } catch (MalformedURLException e) {
                return "output.mp3";
            }
        }
        
        Path path = Paths.get(inputPath);
        String filename = path.getFileName().toString();
        int dotIndex = filename.lastIndexOf('.');
        if (dotIndex > 0) {
            filename = filename.substring(0, dotIndex);
        }
        return path.getParent().resolve(filename + ".mp3").toString();
    }
    
    /**
     * Convert video to MP3 using ffmpeg
     */
    private boolean convertToMp3(String inputPath, String outputPath) {
        ConsoleUtils.printProgress(String.format("Converting: %s → %s", 
            Paths.get(inputPath).getFileName(), Paths.get(outputPath).getFileName()));
        System.out.println();
        
        try {
            ProcessBuilder processBuilder = new ProcessBuilder(
                "ffmpeg",
                "-i", inputPath,
                "-vn",
                "-acodec", "libmp3lame",
                "-ab", "192k",
                "-y",
                outputPath
            );
            
            processBuilder.redirectErrorStream(true);
            Process process = processBuilder.start();
            
            // Read and display progress
            ConsoleUtils.printInfo("🎬 FFmpeg started...");
            
            try (BufferedReader reader = new BufferedReader(
                    new InputStreamReader(process.getInputStream()))) {
                
                String line;
                boolean showProgress = true;
                long startTime = System.currentTimeMillis();
                
                while ((line = reader.readLine()) != null) {
                    // Update progress indicator every few seconds
                    if (showProgress && System.currentTimeMillis() - startTime > 2000) {
                        ConsoleUtils.clearLine();
                        ConsoleUtils.printProgress("Converting... Please wait");
                        System.out.println();
                        showProgress = false;
                    }
                }
            }
            
            boolean finished = process.waitFor(300, TimeUnit.SECONDS);
            
            if (!finished) {
                ConsoleUtils.printError("Conversion timed out after 5 minutes");
                process.destroyForcibly();
                return false;
            }
            
            int exitCode = process.exitValue();
            
            if (exitCode == 0) {
                ConsoleUtils.clearLine();
                ConsoleUtils.printSuccess("Conversion completed successfully!");
                return true;
            } else {
                ConsoleUtils.printError("FFmpeg failed with exit code: " + exitCode);
                return false;
            }
            
        } catch (IOException e) {
            if (e.getMessage().contains("Cannot run program \"ffmpeg\"")) {
                ConsoleUtils.printError("FFmpeg not found. Please make sure FFmpeg is installed and in your PATH.");
            } else {
                ConsoleUtils.printError("Error during conversion: " + e.getMessage());
            }
            return false;
        } catch (InterruptedException e) {
            ConsoleUtils.printError("Conversion was interrupted");
            Thread.currentThread().interrupt();
            return false;
        }
    }
    
    /**
     * Get file size in MB
     */
    private Double getFileSizeMB(String filePath) {
        try {
            Path path = Paths.get(filePath);
            if (Files.exists(path)) {
                long bytes = Files.size(path);
                return bytes / (1024.0 * 1024.0);
            }
        } catch (Exception e) {
            // Ignore errors
        }
        return null;
    }
    
    /**
     * Ask user for working directory
     */
    private String askDirectory() {
        ConsoleUtils.printInfo("📁 Default directory: " + config.getDefaultDir());
        
        if (ConsoleUtils.askConfirmation("Use this as the root directory?", true)) {
            return config.getDefaultDir();
        }
        
        while (true) {
            String customDir = ConsoleUtils.askInput("Enter root directory path");
            
            File dir = new File(customDir);
            if (dir.exists() && dir.isDirectory()) {
                return customDir;
            }
            
            ConsoleUtils.printError("Directory not found: " + customDir);
        }
    }
    
    /**
     * Let user select a video file or enter manual input
     */
    private String selectVideoFile(List<VideoFile> videoFiles, String rootDir) {
        if (videoFiles.isEmpty()) {
            ConsoleUtils.printError("No supported video files found in the directory.");
            ConsoleUtils.printInfo("💡 Supported formats: mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp");
            
            if (!ConsoleUtils.askConfirmation("Would you like to enter a file path or URL manually?", true)) {
                ConsoleUtils.printInfo("👋 Goodbye!");
                System.exit(0);
            }
            
            return ConsoleUtils.askInput("Enter video file path or URL");
        }
        
        ConsoleUtils.printInfo("📹 Found " + videoFiles.size() + " video file(s)");
        
        List<String> choices = new ArrayList<>();
        for (VideoFile vf : videoFiles) {
            choices.add(vf.toString());
        }
        choices.add("📝 Enter file path or URL manually");
        
        int selection = ConsoleUtils.selectFromMenu("Select a video file to convert:", choices);
        
        if (selection == choices.size() - 1) {
            return ConsoleUtils.askInput("Enter video file path or URL");
        }
        
        return videoFiles.get(selection).name();
    }
    
    /**
     * Main application logic
     */
    public void run() {
        ConsoleUtils.printHeader();
        
        try {
            // Get working directory
            String rootDir = askDirectory();
            ConsoleUtils.printInfo("🗂️  Working in: " + rootDir);
            
            // Get video files in directory
            List<VideoFile> videoFiles = getVideoFiles(rootDir);
            
            // Let user select file
            String selectedFile = selectVideoFile(videoFiles, rootDir);
            
            // Determine full input path
            String fullInputPath;
            if (isUrl(selectedFile) || Paths.get(selectedFile).isAbsolute()) {
                fullInputPath = selectedFile;
            } else {
                fullInputPath = Paths.get(rootDir, selectedFile).toString();
            }
            
            // Check if local file exists
            if (!isUrl(fullInputPath)) {
                File inputFile = new File(fullInputPath);
                if (!inputFile.exists()) {
                    ConsoleUtils.printError("File not found: " + fullInputPath);
                    return;
                }
            }
            
            // Generate output path
            String outputPath = getOutputPath(fullInputPath);
            
            System.out.println();
            ConsoleUtils.printInfo("📁 Input: " + fullInputPath);
            ConsoleUtils.printInfo("💾 Output: " + outputPath);
            System.out.println();
            
            // Perform conversion
            if (convertToMp3(fullInputPath, outputPath)) {
                ConsoleUtils.printSuccess("Conversion finished: " + outputPath);
                
                // Show output file size
                Double sizeMB = getFileSizeMB(outputPath);
                if (sizeMB != null) {
                    ConsoleUtils.printFileSize("Output file size", sizeMB);
                }
            } else {
                ConsoleUtils.printError("Conversion failed!");
                System.exit(1);
            }
            
        } catch (Exception e) {
            ConsoleUtils.printError("Unexpected error: " + e.getMessage());
            e.printStackTrace();
            System.exit(1);
        }
    }
}
