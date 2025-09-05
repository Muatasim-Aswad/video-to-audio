use crate::config::Config;
use crate::video_file::VideoFile;
use colored::*;
use indicatif::{ProgressBar, ProgressStyle};
use inquire::{Confirm, Select, Text};
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Stdio;
use tokio::io::{AsyncBufReadExt, BufReader};
use tokio::process::Command;
use url::Url;

/// Main video converter with interactive functionality
pub struct VideoConverter {
    config: Config,
}

impl VideoConverter {
    pub fn new() -> Self {
        Self {
            config: Config::load(),
        }
    }

    /// Check if a string is a valid URL
    fn is_url(input: &str) -> bool {
        if let Ok(url) = Url::parse(input) {
            matches!(url.scheme(), "http" | "https")
        } else {
            false
        }
    }

    /// Get all video files in a directory
    fn get_video_files(&self, directory: &str) -> Result<Vec<VideoFile>, Box<dyn std::error::Error>> {
        let mut video_files = Vec::new();
        let dir_path = Path::new(directory);

        if !dir_path.exists() || !dir_path.is_dir() {
            return Ok(video_files);
        }

        let entries = fs::read_dir(dir_path)?;
        for entry in entries {
            let entry = entry?;
            let path = entry.path();

            if path.is_file() {
                if let Ok(video_file) = VideoFile::new(&path) {
                    if video_file.is_supported_video() {
                        video_files.push(video_file);
                    }
                }
            }
        }

        // Sort by filename (case-insensitive)
        video_files.sort_by(|a, b| a.name.to_lowercase().cmp(&b.name.to_lowercase()));

        Ok(video_files)
    }

    /// Generate output MP3 path based on input path
    fn get_output_path(&self, input_path: &str) -> String {
        if Self::is_url(input_path) {
            if let Ok(url) = Url::parse(input_path) {
                let path = url.path();
                let filename = Path::new(path)
                    .file_stem()
                    .and_then(|s| s.to_str())
                    .unwrap_or("output");
                return format!("{}.mp3", filename);
            }
            return "output.mp3".to_string();
        }

        let path = Path::new(input_path);
        let filename = path
            .file_stem()
            .and_then(|s| s.to_str())
            .unwrap_or("output");

        if let Some(parent) = path.parent() {
            parent.join(format!("{}.mp3", filename)).to_string_lossy().to_string()
        } else {
            format!("{}.mp3", filename)
        }
    }

    /// Convert video to MP3 using ffmpeg
    async fn convert_to_mp3(&self, input_path: &str, output_path: &str) -> Result<bool, Box<dyn std::error::Error>> {
        println!("{}", format!("🔄 Converting: {} → {}", 
            Path::new(input_path).file_name().unwrap_or_default().to_string_lossy(),
            Path::new(output_path).file_name().unwrap_or_default().to_string_lossy()
        ).blue());

        let pb = ProgressBar::new_spinner();
        pb.set_style(
            ProgressStyle::default_spinner()
                .template("{spinner:.green} {msg}")
                .unwrap()
        );
        pb.set_message("🎬 FFmpeg started...");

        let mut cmd = Command::new("ffmpeg")
            .args(&[
                "-i", input_path,
                "-vn",
                "-acodec", "libmp3lame",
                "-ab", "192k",
                "-y",
                output_path,
            ])
            .stdout(Stdio::piped())
            .stderr(Stdio::piped())
            .spawn()?;

        // Read stderr for progress information
        if let Some(stderr) = cmd.stderr.take() {
            let reader = BufReader::new(stderr);
            let mut lines = reader.lines();

            tokio::spawn(async move {
                while let Ok(Some(_line)) = lines.next_line().await {
                    // Could parse FFmpeg progress here if needed
                }
            });
        }

        pb.set_message("Converting... Please wait");
        
        let status = cmd.wait().await?;
        pb.finish_and_clear();

        if status.success() {
            println!("{}", "✅ Conversion completed successfully!".green());
            Ok(true)
        } else {
            match status.code() {
                Some(code) => println!("{}", format!("❌ FFmpeg failed with exit code: {}", code).red()),
                None => println!("{}", "❌ FFmpeg process was terminated".red()),
            }
            Ok(false)
        }
    }

    /// Get file size in MB
    fn get_file_size_mb(&self, file_path: &str) -> Option<f64> {
        if let Ok(metadata) = fs::metadata(file_path) {
            Some(metadata.len() as f64 / (1024.0 * 1024.0))
        } else {
            None
        }
    }

    /// Ask user for working directory
    fn ask_directory(&self) -> Result<String, Box<dyn std::error::Error>> {
        println!("{}", format!("📁 Default directory: {}", self.config.default_dir).blue());

        let use_default = Confirm::new("Use this as the root directory?")
            .with_default(true)
            .prompt()?;

        if use_default {
            Ok(self.config.default_dir.clone())
        } else {
            loop {
                let custom_dir = Text::new("Enter root directory path:")
                    .prompt()?;

                let path = Path::new(&custom_dir);
                if path.exists() && path.is_dir() {
                    return Ok(custom_dir);
                }

                println!("{}", format!("❌ Directory not found: {}", custom_dir).red());
            }
        }
    }

    /// Let user select a video file or enter manual input
    fn select_video_file(&self, video_files: &[VideoFile], _root_dir: &str) -> Result<String, Box<dyn std::error::Error>> {
        if video_files.is_empty() {
            println!("{}", "❌ No supported video files found in the directory.".red());
            println!("{}", "💡 Supported formats: mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp".blue());

            let enter_manual = Confirm::new("Would you like to enter a file path or URL manually?")
                .with_default(true)
                .prompt()?;

            if !enter_manual {
                println!("{}", "👋 Goodbye!".blue());
                std::process::exit(0);
            }

            let manual_path = Text::new("Enter video file path or URL:")
                .prompt()?;

            return Ok(manual_path);
        }

        println!("{}", format!("📹 Found {} video file(s)", video_files.len()).blue());

        let mut choices: Vec<String> = video_files.iter().map(|vf| vf.to_string()).collect();
        choices.push("📝 Enter file path or URL manually".to_string());

        let selection = Select::new("Select a video file to convert:", choices)
            .prompt()?;

        if selection == "📝 Enter file path or URL manually" {
            let manual_path = Text::new("Enter video file path or URL:")
                .prompt()?;
            Ok(manual_path)
        } else {
            // Find the selected video file
            for video_file in video_files {
                if video_file.to_string() == selection {
                    return Ok(video_file.name.clone());
                }
            }
            Err("Selected file not found".into())
        }
    }

    /// Main application logic
    pub async fn run(&self) -> Result<(), Box<dyn std::error::Error>> {
        // Print header
        println!("{}", "╔══════════════════════════════════════════╗".cyan());
        println!("{}", "║          Video to Audio Converter       ║".cyan());
        println!("{}", "║                 (Rust)                   ║".cyan());
        println!("{}", "╚══════════════════════════════════════════╝".cyan());
        println!();

        // Get working directory
        let root_dir = self.ask_directory()?;
        println!("{}", format!("🗂️  Working in: {}", root_dir).blue());

        // Get video files in directory
        let video_files = self.get_video_files(&root_dir)?;

        // Let user select file
        let selected_file = self.select_video_file(&video_files, &root_dir)?;

        // Determine full input path
        let full_input_path = if Self::is_url(&selected_file) || Path::new(&selected_file).is_absolute() {
            selected_file
        } else {
            Path::new(&root_dir).join(&selected_file).to_string_lossy().to_string()
        };

        // Check if local file exists
        if !Self::is_url(&full_input_path) {
            let input_path = Path::new(&full_input_path);
            if !input_path.exists() {
                println!("{}", format!("❌ File not found: {}", full_input_path).red());
                return Ok(());
            }
        }

        // Generate output path
        let output_path = self.get_output_path(&full_input_path);

        println!();
        println!("{}", format!("📁 Input: {}", full_input_path).blue());
        println!("{}", format!("💾 Output: {}", output_path).blue());
        println!();

        // Perform conversion
        match self.convert_to_mp3(&full_input_path, &output_path).await {
            Ok(true) => {
                println!("{}", format!("✅ Conversion finished: {}", output_path).green());

                // Show output file size
                if let Some(size_mb) = self.get_file_size_mb(&output_path) {
                    println!("{}", format!("📊 Output file size: {:.2} MB", size_mb).yellow());
                }
            }
            Ok(false) => {
                println!("{}", "❌ Conversion failed!".red());
                std::process::exit(1);
            }
            Err(e) => {
                if e.to_string().contains("No such file or directory") 
                    || e.to_string().contains("program not found") {
                    println!("{}", "❌ FFmpeg not found. Please make sure FFmpeg is installed and in your PATH.".red());
                } else {
                    println!("{}", format!("❌ Error during conversion: {}", e).red());
                }
                std::process::exit(1);
            }
        }

        Ok(())
    }
}
