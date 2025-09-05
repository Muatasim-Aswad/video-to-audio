package main

import (
	"fmt"
	"log"
	"net/url"
	"os"
	"os/exec"
	"path/filepath"
	"sort"
	"strings"

	"github.com/joho/godotenv"
	"github.com/manifoldco/promptui"
)

// VideoFile represents a video file with its metadata
type VideoFile struct {
	Name        string
	Path        string
	SizeMB      float64
	Extension   string
}

// Config holds application configuration
type Config struct {
	DefaultDir string
}

// loadConfig loads configuration from .env file
func loadConfig() *Config {
	config := &Config{
		DefaultDir: "/Users/hackyourfuture/Downloads", // fallback default
	}
	
	// Try to load .env file from parent directory
	envPath := filepath.Join("..", ".env")
	if err := godotenv.Load(envPath); err == nil {
		if defaultDir := os.Getenv("DEFAULT_DIR"); defaultDir != "" {
			config.DefaultDir = defaultDir
		}
	}
	
	return config
}

// isURL checks if a string is a valid URL
func isURL(str string) bool {
	u, err := url.Parse(str)
	return err == nil && u.Scheme != "" && u.Host != ""
}

// getVideoFiles scans directory for supported video files
func getVideoFiles(directory string) ([]VideoFile, error) {
	var videoFiles []VideoFile
	supportedExts := []string{".mp4", ".avi", ".mov", ".mkv", ".flv", ".wmv", ".webm", ".m4v", ".3gp"}
	
	entries, err := os.ReadDir(directory)
	if err != nil {
		return nil, err
	}
	
	for _, entry := range entries {
		if entry.IsDir() {
			continue
		}
		
		ext := strings.ToLower(filepath.Ext(entry.Name()))
		isSupported := false
		for _, supportedExt := range supportedExts {
			if ext == supportedExt {
				isSupported = true
				break
			}
		}
		
		if isSupported {
			fullPath := filepath.Join(directory, entry.Name())
			info, err := entry.Info()
			if err != nil {
				continue
			}
			
			sizeMB := float64(info.Size()) / (1024 * 1024)
			
			videoFiles = append(videoFiles, VideoFile{
				Name:      entry.Name(),
				Path:      fullPath,
				SizeMB:    sizeMB,
				Extension: ext,
			})
		}
	}
	
	// Sort files alphabetically
	sort.Slice(videoFiles, func(i, j int) bool {
		return videoFiles[i].Name < videoFiles[j].Name
	})
	
	return videoFiles, nil
}

// getOutputPath generates output MP3 path based on input
func getOutputPath(inputPath string) string {
	if isURL(inputPath) {
		u, _ := url.Parse(inputPath)
		baseName := filepath.Base(u.Path)
		if baseName == "" || baseName == "/" {
			baseName = "output"
		}
		nameWithoutExt := strings.TrimSuffix(baseName, filepath.Ext(baseName))
		return nameWithoutExt + ".mp3"
	}
	
	dir := filepath.Dir(inputPath)
	baseName := filepath.Base(inputPath)
	nameWithoutExt := strings.TrimSuffix(baseName, filepath.Ext(baseName))
	return filepath.Join(dir, nameWithoutExt+".mp3")
}

// convertToMP3 performs the actual conversion using ffmpeg
func convertToMP3(inputPath, outputPath string) error {
	fmt.Printf("🔄 Converting: %s → %s\n", filepath.Base(inputPath), filepath.Base(outputPath))
	
	cmd := exec.Command("ffmpeg", "-i", inputPath, "-vn", "-acodec", "libmp3lame", "-ab", "192k", outputPath)
	
	// Show progress by displaying stderr
	cmd.Stderr = os.Stderr
	
	err := cmd.Run()
	if err != nil {
		return fmt.Errorf("ffmpeg error: %v", err)
	}
	
	return nil
}

// getFileSize returns file size in MB
func getFileSize(path string) (float64, error) {
	info, err := os.Stat(path)
	if err != nil {
		return 0, err
	}
	return float64(info.Size()) / (1024 * 1024), nil
}

func main() {
	config := loadConfig()
	
	fmt.Println("🎵 Video to Audio Converter (Go)")
	fmt.Println("=================================")
	
	// Ask about default directory
	fmt.Printf("📁 Default directory: %s\n", config.DefaultDir)
	
	prompt := promptui.Prompt{
		Label:     "Use this as the root directory",
		IsConfirm: true,
		Default:   "y",
	}
	
	useDefault := true
	if _, err := prompt.Run(); err != nil {
		useDefault = false
	}
	
	rootDir := config.DefaultDir
	if !useDefault {
		prompt := promptui.Prompt{
			Label: "Enter root directory path",
			Validate: func(input string) error {
				if _, err := os.Stat(input); os.IsNotExist(err) {
					return fmt.Errorf("directory not found: %s", input)
				}
				return nil
			},
		}
		
		var err error
		rootDir, err = prompt.Run()
		if err != nil {
			log.Fatal("Error getting directory path:", err)
		}
	}
	
	fmt.Printf("\n🗂️  Working in: %s\n", rootDir)
	
	// Get video files in directory
	videoFiles, err := getVideoFiles(rootDir)
	if err != nil {
		log.Fatal("Error reading directory:", err)
	}
	
	var selectedFile string
	
	if len(videoFiles) == 0 {
		fmt.Println("❌ No supported video files found in the directory.")
		fmt.Println("💡 Supported formats: mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp")
		
		// Offer manual input
		manualPrompt := promptui.Prompt{
			Label:     "Would you like to enter a file path or URL manually",
			IsConfirm: true,
			Default:   "y",
		}
		
		if _, err := manualPrompt.Run(); err != nil {
			fmt.Println("👋 Goodbye!")
			return
		}
		
		inputPrompt := promptui.Prompt{
			Label: "Enter video file path or URL",
			Validate: func(input string) error {
				if strings.TrimSpace(input) == "" {
					return fmt.Errorf("please enter a file path or URL")
				}
				return nil
			},
		}
		
		selectedFile, err = inputPrompt.Run()
		if err != nil {
			log.Fatal("Error getting input:", err)
		}
	} else {
		fmt.Printf("📹 Found %d video file(s)\n", len(videoFiles))
		
		// Create selection options
		items := make([]string, len(videoFiles)+1)
		for i, file := range videoFiles {
			items[i] = fmt.Sprintf("%s (%.1f MB)", file.Name, file.SizeMB)
		}
		items[len(videoFiles)] = "📝 Enter file path or URL manually"
		
		selectPrompt := promptui.Select{
			Label:    "Select a video file to convert",
			Items:    items,
			Size:     10,
		}
		
		index, _, err := selectPrompt.Run()
		if err != nil {
			log.Fatal("Error in selection:", err)
		}
		
		if index == len(videoFiles) {
			// Manual input selected
			inputPrompt := promptui.Prompt{
				Label: "Enter video file path or URL",
				Validate: func(input string) error {
					if strings.TrimSpace(input) == "" {
						return fmt.Errorf("please enter a file path or URL")
					}
					return nil
				},
			}
			
			selectedFile, err = inputPrompt.Run()
			if err != nil {
				log.Fatal("Error getting input:", err)
			}
		} else {
			selectedFile = videoFiles[index].Name
		}
	}
	
	// Determine full input path
	var fullInputPath string
	if isURL(selectedFile) || filepath.IsAbs(selectedFile) {
		fullInputPath = selectedFile
	} else {
		fullInputPath = filepath.Join(rootDir, selectedFile)
	}
	
	// Check if local file exists
	if !isURL(fullInputPath) {
		if _, err := os.Stat(fullInputPath); os.IsNotExist(err) {
			fmt.Printf("❌ File not found: %s\n", fullInputPath)
			return
		}
	}
	
	// Generate output path
	outputPath := getOutputPath(fullInputPath)
	
	fmt.Printf("\n📁 Input: %s\n", fullInputPath)
	fmt.Printf("💾 Output: %s\n", outputPath)
	fmt.Println()
	
	// Perform conversion
	if err := convertToMP3(fullInputPath, outputPath); err != nil {
		fmt.Printf("❌ Conversion failed: %v\n", err)
		return
	}
	
	fmt.Printf("✅ Conversion finished: %s\n", outputPath)
	
	// Show output file size
	if size, err := getFileSize(outputPath); err == nil {
		fmt.Printf("📊 Output file size: %.2f MB\n", size)
	}
}