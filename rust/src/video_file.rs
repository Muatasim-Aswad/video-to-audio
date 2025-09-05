use std::fmt;
use std::path::Path;

/// Represents a video file with metadata
#[derive(Debug, Clone)]
pub struct VideoFile {
    pub name: String,
    pub path: String,
    pub size_mb: f64,
    pub extension: String,
}

impl VideoFile {
    /// Create a new VideoFile from a file path
    pub fn new(path: &Path) -> Result<Self, std::io::Error> {
        let metadata = std::fs::metadata(path)?;
        let size_mb = metadata.len() as f64 / (1024.0 * 1024.0);
        
        let name = path
            .file_name()
            .and_then(|n| n.to_str())
            .unwrap_or("unknown")
            .to_string();
        
        let extension = path
            .extension()
            .and_then(|ext| ext.to_str())
            .unwrap_or("")
            .to_lowercase();
        
        Ok(VideoFile {
            name,
            path: path.to_string_lossy().to_string(),
            size_mb,
            extension,
        })
    }
    
    /// Check if the file has a supported video extension
    pub fn is_supported_video(&self) -> bool {
        const SUPPORTED_EXTENSIONS: &[&str] = &[
            "mp4", "avi", "mov", "mkv", "flv", "wmv", "webm", "m4v", "3gp"
        ];
        
        SUPPORTED_EXTENSIONS.contains(&self.extension.as_str())
    }
}

impl fmt::Display for VideoFile {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{} ({:.1} MB)", self.name, self.size_mb)
    }
}
