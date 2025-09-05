package com.hackyourfuture.videoconverter;

/**
 * Represents a video file with metadata.
 * 
 * @param name The filename
 * @param path The full file path
 * @param sizeMB The file size in megabytes
 * @param extension The file extension
 */
public record VideoFile(String name, String path, double sizeMB, String extension) {
    
    @Override
    public String toString() {
        return String.format("%s (%.1f MB)", name, sizeMB);
    }
    
    /**
     * Create a VideoFile from a java.io.File object
     */
    public static VideoFile fromFile(java.io.File file) {
        double sizeMB = file.length() / (1024.0 * 1024.0);
        String name = file.getName();
        String extension = getFileExtension(name);
        
        return new VideoFile(name, file.getAbsolutePath(), sizeMB, extension);
    }
    
    /**
     * Get the file extension from a filename
     */
    private static String getFileExtension(String filename) {
        int dotIndex = filename.lastIndexOf('.');
        return (dotIndex > 0) ? filename.substring(dotIndex).toLowerCase() : "";
    }
}
