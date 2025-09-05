# Video to Audio Converter

A multi-language video-to-audio converter that supports interactive file selection and conversion using FFmpeg. Available in Node.js, Go, Python, C#, Java, Rust, and C++.

## Features

- Convert MP4 videos to MP3 audio format
- High-quality audio extraction using libmp3lame codec
- Interactive file selection with rich UI
- Support for multiple programming languages
- Cross-platform compatibility

## Available Versions

This project includes implementations in seven different programming languages:

### Node.js Version (`/node/`)

- Interactive file selection with modern UI
- Uses `@inquirer/prompts` for user interaction
- ES modules support
- Progress display during conversion

### Go Version (`/go/`)

- Compiled binary for easy distribution
- Uses `promptui` for interactive menus
- Can be built to a single executable file
- Cross-platform support

### Python Version (`/python/`)

- Pure standard library implementation
- No external dependencies required
- Class-based design for better organization
- Cross-platform compatibility

### C# Version (`/csharp/`)

- .NET console application
- Rich UI with `Spectre.Console`
- Self-contained executable option
- Modern C# features with record types

### Java Version (`/java/`)

- Pure Java implementation (Java 17+)
- No external dependencies required
- ANSI color support for rich console UI
- Cross-platform JAR file distribution

### Rust Version (`/rust/`)

- Memory-safe and fast implementation
- Async/await support with Tokio runtime
- Rich colored console UI with modern prompts
- Highly optimized binary (~980KB)

### C++ Version (`/cpp/`)

- Native performance with modern C++17 features
- CMake build system for cross-platform compilation
- Memory-safe RAII design patterns
- Ultra-compact binary (~188KB)

All versions support:

- ✅ Interactive directory selection
- ✅ File discovery and selection menus
- ✅ Manual file path/URL input
- ✅ Progress display during conversion
- ✅ Shared `.env` configuration
- ✅ Support for multiple video formats (mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp)

## Installation

### Prerequisites

All versions require **FFmpeg** to be installed and available in your system PATH.

#### Installing FFmpeg

**macOS (using Homebrew):**

```bash
brew install ffmpeg
```

**macOS (Static Binary - Recommended if Homebrew fails):**

```bash
# Download from evermeet.cx (maintained builds)
curl -O https://evermeet.cx/ffmpeg/ffmpeg-N-120851-g7a774829f9-tessus.7z
# Extract and move to /usr/local/bin/
```

**Ubuntu/Debian:**

```bash
sudo apt update
sudo apt install ffmpeg
```

**Windows:**
Download from [https://ffmpeg.org/download.html](https://ffmpeg.org/download.html) and add to your PATH.

**Verify Installation:**

```bash
ffmpeg -version
```

### Node.js Version

```bash
cd node/
npm install
node convert.js
```

### Go Version

```bash
cd go/
go mod init video-converter
go mod tidy
go run convert.go

# Or build executable:
go build -o convert convert.go
./convert
```

### Python Version

```bash
cd python/
python3 convert.py
```

### C# Version

```bash
cd csharp/VideoToAudioConverter/
dotnet run

# Or build self-contained executable:
./build.sh
```

### Java Version

```bash
cd java/
./run.sh

# Or build and run manually:
./build.sh
java -jar video-to-audio-converter.jar
```

### Rust Version

```bash
cd rust/
./run.sh

# Or build and run manually:
./build.sh
./target/release/convert
```

### C++ Version

```bash
cd cpp/
./run.sh

# Or build and run manually:
./build.sh
./build/bin/convert
```

## Configuration

Create a `.env` file in the root directory to set your default video directory:

```env
DEFAULT_DIR=/Users/yourusername/Downloads
```

All versions will automatically read this configuration.

## Project Structure

```
video-to-audio/
├── .env                    # Configuration (default directory)
├── README.md              # This documentation
├── node/                  # Node.js version
│   ├── convert.js         #   Interactive converter script
│   └── package.json       #   Node.js dependencies
├── go/                    # Go version
│   ├── convert.go         #   Go converter with promptui
│   ├── go.mod             #   Go module file
│   └── go.sum             #   Go dependencies
├── python/                # Python version
│   └── convert.py         #   Pure standard library converter
├── csharp/                # C# version
│   └── VideoToAudioConverter/
│       ├── Program.cs     #   Main C# application
│       ├── VideoToAudioConverter.csproj
│       └── build.sh       #   Build script for executable
├── java/                  # Java version
    ├── src/main/java/com/hackyourfuture/videoconverter/
    │   ├── VideoConverterApp.java    # Main entry point
    │   ├── VideoConverter.java       # Core logic
    │   ├── VideoFile.java            # File record
    │   ├── Config.java               # Configuration
    │   └── ConsoleUtils.java         # Console utilities
    ├── video-to-audio-converter.jar  # Runnable JAR
    ├── build.sh           #   Build script
    └── run.sh             #   Run script
└── rust/                  # Rust version
    ├── src/
    │   ├── main.rs        #   Application entry point
    │   ├── converter.rs   #   Core async conversion logic
    │   ├── video_file.rs  #   VideoFile struct
    │   └── config.rs      #   Configuration management
    ├── target/release/convert  # Optimized binary
    ├── build.sh           #   Build script
    └── run.sh             #   Run script
└── cpp/                   # C++ version
    ├── src/
    │   ├── main.cpp       #   Application entry point
    │   ├── VideoConverter.cpp # Core conversion logic
    │   ├── VideoFile.cpp  #   VideoFile class
    │   ├── Config.cpp     #   Configuration management
    │   └── ConsoleUtils.cpp # Console utilities
    ├── include/           #   Header files
    ├── build/bin/convert  #   Compiled executable
    ├── CMakeLists.txt     #   CMake build configuration
    ├── build.sh           #   Build script
    └── run.sh             #   Run script
    ├── Cargo.toml         #   Dependencies and build config
    ├── build.sh           #   Build script
    └── run.sh             #   Run script
    ├── build.sh           #   Build script
    └── run.sh             #   Run script
```

## How It Works

All versions follow the same workflow:

1. **Configuration Loading**: Read default directory from `.env` file
2. **Directory Selection**: Interactive prompt for working directory
3. **File Discovery**: Scan directory for supported video formats
4. **File Selection**: Present numbered menu of available files
5. **Conversion**: Use FFmpeg to extract audio as MP3
6. **Progress Display**: Show conversion progress with visual feedback
7. **Completion**: Display output file information

### Supported Formats

**Input**: mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp  
**Output**: MP3 (192k bitrate, libmp3lame codec)

## Language-Specific Features

### Node.js

- Modern ES modules syntax
- Async/await with promise-based ffmpeg wrapper
- Rich interactive prompts with `@inquirer/prompts`
- Automatic dependency management with npm

### Go

- Statically compiled binary (no dependencies needed)
- Fast performance with goroutines
- Beautiful terminal UI with `promptui`
- Cross-platform executable generation

### Python

- Zero external dependencies (pure standard library)
- Subprocess-based FFmpeg integration
- Class-based object-oriented design
- Universal Python compatibility (3.7+)

### C#

- Modern .NET features (records, pattern matching)
- Rich console UI with `Spectre.Console`
- Self-contained deployment option
- Cross-platform .NET runtime support

### Java

- Pure Java implementation with no external dependencies
- ANSI color support for rich console output
- Cross-platform JAR file distribution
- Modern Java features (Java 17+)

### Rust

- Memory-safe systems programming language
- Async/await support with Tokio runtime
- Rich colored console UI with modern interactive prompts
- Highly optimized binary with excellent performance

### C++

- Native performance with modern C++17 features
- Memory-safe RAII design patterns and smart pointers
- CMake build system for cross-platform compilation
- Ultra-compact binary size with excellent cold start time

## Usage Examples

### Interactive Mode (All Versions)

1. Run the program
2. Choose or confirm the working directory
3. Select a video file from the presented list
4. Watch the conversion progress
5. Find your MP3 in the same directory

### Manual Input Mode

- When no files are found or you select manual input
- Enter a full file path or URL
- Supports remote URLs for video conversion

## Troubleshooting

### FFmpeg Not Found

- **Error**: "FFmpeg not found" or "command not found"
- **Solution**: Install FFmpeg and ensure it's in your system PATH
- **macOS**: If Homebrew fails, use static binary from evermeet.cx
- **Test**: Run `ffmpeg -version` in terminal

### Permission Denied

- **Error**: Cannot read directory or write output file
- **Solution**: Check file and directory permissions
- **Command**: `chmod +x ./build.sh` for build scripts

### File Not Found

- **Error**: Selected file doesn't exist
- **Solution**: Ensure the file path is correct and file exists
- **Note**: Files may have been moved or renamed since directory scan

### Conversion Failed

- **Error**: FFmpeg exits with non-zero code
- **Solution**: Check if input file is valid/not corrupted
- **Debug**: Run FFmpeg manually to see detailed error messages

### Build Issues

#### Go Build Errors

```bash
cd go/
go mod init video-converter
go mod tidy
go build -o convert convert.go
```

#### C# Build Errors

```bash
cd csharp/VideoToAudioConverter/
dotnet clean
dotnet restore
dotnet build
```

#### Node.js Module Errors

```bash
cd node/
rm -rf node_modules package-lock.json
npm install
```

## Performance Comparison

| Language | Binary Size | Cold Start | Memory Usage | Dependencies   |
| -------- | ----------- | ---------- | ------------ | -------------- |
| C++      | ~188KB      | <50ms      | ~10MB        | None (static)  |
| Rust     | ~980KB      | <100ms     | ~15MB        | None (static)  |
| Java     | ~12KB       | <1000ms    | ~20MB        | Java runtime   |
| Go       | ~15MB       | <100ms     | ~10MB        | None (static)  |
| C#       | ~65MB       | ~200ms     | ~25MB        | .NET Runtime   |
| Python   | ~0MB        | ~300ms     | ~15MB        | Python runtime |
| Node.js  | ~200MB      | ~500ms     | ~40MB        | Node + modules |

## Contributing

This project welcomes contributions in any of the supported languages:

1. Fork the repository
2. Choose your preferred language implementation
3. Create a feature branch (`git checkout -b feature/new-feature`)
4. Make your changes following the existing code style
5. Test thoroughly with various video formats
6. Commit your changes (`git commit -m 'Add feature: description'`)
7. Push to your branch (`git push origin feature/new-feature`)
8. Open a Pull Request

### Code Style Guidelines

- **Node.js**: ES modules, async/await, meaningful variable names
- **Go**: Go fmt, error handling, structured logging
- **Python**: PEP 8, type hints where helpful, docstrings
- **C#**: Standard C# conventions, XML documentation, async patterns

## License

MIT License - see the project repository for details.

## Support

For issues, questions, or contributions:

- Open an issue on GitHub
- Provide your system info (OS, language version, FFmpeg version)
- Include error messages and steps to reproduce
