# Java Video to Audio Converter

A Java implementation of the interactive video-to-audio converter with file selection menus and progress display.

## Features

- ✅ Interactive directory and file selection
- ✅ Support for multiple video formats (mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp)
- ✅ Manual file path/URL input option
- ✅ Progress display during conversion
- ✅ Configuration via `.env` file
- ✅ Color-coded console output (ANSI colors)
- ✅ Pure Java implementation (no external dependencies)

## Requirements

- **Java 17 or higher** (tested with Java 23)
- **FFmpeg** installed and available in system PATH

## Quick Start

### Option 1: Using the run script (recommended)

```bash
./run.sh
```

### Option 2: Build and run manually

```bash
# Build the application
./build.sh

# Run the JAR file
java -jar video-to-audio-converter.jar
```

## Build Process

The project uses a simple build process with `javac` and `jar` (no Maven required):

1. **Compilation**: Java source files are compiled to `.class` files
2. **JAR Creation**: A runnable JAR file is created with the main class specified
3. **Execution**: The JAR can be run directly with `java -jar`

### Build Directory Structure

```
java/
├── src/main/java/com/hackyourfuture/videoconverter/
│   ├── VideoConverterApp.java     # Main application entry point
│   ├── VideoConverter.java        # Core conversion logic
│   ├── VideoFile.java             # Video file record
│   ├── Config.java                # Configuration management
│   └── ConsoleUtils.java          # Console UI utilities
├── build/classes/                 # Compiled .class files
├── video-to-audio-converter.jar   # Runnable JAR file
├── build.sh                       # Build script
├── run.sh                         # Run script
└── README.md                      # This file
```

## Architecture

### Classes Overview

- **VideoConverterApp**: Main entry point with proper shutdown hooks
- **VideoConverter**: Core logic for file discovery, selection, and conversion
- **VideoFile**: Record class representing video file metadata
- **Config**: Configuration management with `.env` file support
- **ConsoleUtils**: Pure Java console utilities with ANSI color support

### Key Features

1. **Zero External Dependencies**: Uses only standard Java libraries
2. **ANSI Color Support**: Rich console output with colors and emojis
3. **Cross-Platform**: Runs on any system with Java 17+
4. **Interactive Menus**: Numbered selection lists with validation
5. **Progress Feedback**: Real-time conversion progress display
6. **Error Handling**: Comprehensive error handling and user-friendly messages

## Usage Examples

### Interactive File Selection

1. Run the application: `./run.sh`
2. Confirm or choose working directory
3. Select from available video files
4. Watch conversion progress
5. Find your MP3 in the same directory

### Manual File Input

- Select "Enter file path or URL manually" from the menu
- Provide full path to video file or HTTP/HTTPS URL
- Conversion proceeds with the specified input

## Troubleshooting

### Java Version Issues

```bash
# Check Java version
java -version

# Ensure Java 17+ is installed
# On macOS with Homebrew:
brew install openjdk@17
```

### FFmpeg Not Found

```bash
# Test FFmpeg installation
ffmpeg -version

# Install FFmpeg (macOS)
brew install ffmpeg
```

### Compilation Errors

```bash
# Clean and rebuild
rm -rf build/ video-to-audio-converter.jar
./build.sh
```

### Permission Denied

```bash
# Make scripts executable
chmod +x build.sh run.sh
```

## Performance

- **JAR Size**: ~50KB (no external dependencies)
- **Memory Usage**: ~20MB typical runtime
- **Startup Time**: <1 second cold start
- **Conversion Speed**: Limited by FFmpeg processing

## Customization

The application can be easily extended:

### Adding New Video Formats

Edit `SUPPORTED_EXTENSIONS` in `VideoConverter.java`:

```java
private static final List<String> SUPPORTED_EXTENSIONS = Arrays.asList(
    ".mp4", ".avi", ".mov", ".mkv", ".flv", ".wmv", ".webm", ".m4v", ".3gp",
    ".your_new_format"  // Add here
);
```

### Changing FFmpeg Parameters

Modify the `ProcessBuilder` arguments in `convertToMp3()`:

```java
ProcessBuilder processBuilder = new ProcessBuilder(
    "ffmpeg",
    "-i", inputPath,
    "-vn",                    // No video
    "-acodec", "libmp3lame",  // MP3 codec
    "-ab", "320k",            // Higher bitrate
    "-y",                     // Overwrite output
    outputPath
);
```

### Custom Console Colors

Modify color constants in `ConsoleUtils.java`:

```java
public static final String GREEN = "\u001B[32m";  // Customize colors
public static final String BLUE = "\u001B[34m";
// Add more color constants as needed
```

## Contributing

1. Fork the repository
2. Make changes following Java conventions
3. Test thoroughly with various video formats
4. Submit a pull request

### Code Style

- Follow standard Java naming conventions
- Use meaningful variable and method names
- Add Javadoc comments for public methods
- Keep methods focused and concise

## License

MIT License - see the project repository for details.
