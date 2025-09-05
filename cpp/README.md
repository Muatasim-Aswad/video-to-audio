# C++ Video to Audio Converter

A high-performance C++ implementation of the interactive video-to-audio converter using modern C++17 features, CMake build system, and cross-platform filesystem support.

## Features

- ✅ Interactive directory and file selection with colored UI
- ✅ Support for multiple video formats (mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp)
- ✅ Manual file path/URL input option
- ✅ Progress display during conversion
- ✅ Configuration via `.env` file
- ✅ Rich colored console output with ANSI colors
- ✅ Modern C++17 features (filesystem, smart pointers, auto)
- ✅ Cross-platform compatibility (Linux, macOS, Windows)
- ✅ Memory-safe implementation with RAII

## Requirements

- **C++ Compiler**: GCC 7+, Clang 5+, or MSVC 2019+ with C++17 support
- **CMake**: Version 3.16 or higher
- **Build Tools**: Make (Unix/Linux) or Visual Studio (Windows)
- **FFmpeg**: Installed and available in system PATH

## Quick Start

### Option 1: Using the run script (recommended)

```bash
./run.sh
```

### Option 2: Build and run manually

```bash
# Build the application
./build.sh

# Run the executable
./build/bin/convert
```

### Option 3: Using CMake directly

```bash
# Create build directory
mkdir build && cd build

# Configure (Release mode for optimization)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make -j$(nproc)

# Run
./bin/convert
```

## Build Process

The project uses CMake for cross-platform building with the following features:

- **C++17 Standard**: Modern C++ features including `std::filesystem`
- **Cross-Platform**: Automatic detection of filesystem library availability
- **Optimized Builds**: Release mode with `-O3` optimization and symbol stripping
- **Dependency Detection**: Automatic fallback to `std::experimental::filesystem` if needed
- **Parallel Compilation**: Multi-threaded builds with `-j$(nproc)`

### Project Structure

```
cpp/
├── src/
│   ├── main.cpp           # Application entry point with signal handling
│   ├── VideoConverter.cpp # Core conversion logic and user interaction
│   ├── VideoFile.cpp      # Video file metadata and utilities
│   ├── Config.cpp         # Configuration management (.env support)
│   └── ConsoleUtils.cpp   # Console UI utilities with ANSI colors
├── include/
│   ├── VideoConverter.h   # VideoConverter class definition
│   ├── VideoFile.h        # VideoFile class definition
│   ├── Config.h           # Config class definition
│   └── ConsoleUtils.h     # ConsoleUtils class definition
├── build/
│   └── bin/convert        # Compiled executable (after build)
├── CMakeLists.txt         # CMake build configuration
├── build.sh               # Build script
├── run.sh                 # Run script
└── README.md              # This file
```

## Architecture

### Modern C++ Features

The C++ version leverages modern C++17 features:

- **std::filesystem**: Cross-platform file and directory operations
- **Smart Pointers**: `std::unique_ptr` for automatic memory management
- **RAII**: Resource Acquisition Is Initialization for exception safety
- **Auto Type Deduction**: Cleaner, more maintainable code
- **Range-based Loops**: Simplified iteration over containers
- **String Literals**: Raw string literals for regex patterns

### Design Patterns

1. **RAII**: Automatic resource management prevents memory leaks
2. **Factory Pattern**: `VideoFile::fromPath()` for object creation
3. **Singleton-like Config**: Single configuration instance per application
4. **Command Pattern**: Encapsulated FFmpeg operations
5. **Strategy Pattern**: Different filesystem implementations based on availability

### Key Features

1. **Fast Performance**: Compiled native code with optimizations
2. **Memory Safe**: RAII and smart pointers prevent memory issues
3. **Cross-Platform**: Works on Linux, macOS, and Windows
4. **Modern UI**: ANSI colored output with emojis and progress indicators
5. **Robust Error Handling**: Exception-safe operations throughout

## Usage Examples

### Interactive File Selection

1. Run the application: `./run.sh`
2. Confirm or choose working directory
3. Select from available video files using numbered menu
4. Watch conversion progress with FFmpeg output
5. Find your MP3 in the same directory

### Manual File Input

- Select "Enter file path or URL manually" from the menu
- Provide full path to video file or HTTP/HTTPS URL
- Conversion proceeds with real-time progress display

### Configuration

Create a `.env` file in the parent directory:

```env
DEFAULT_DIR=/path/to/your/videos
```

## Performance

The C++ version offers excellent performance characteristics:

- **Binary Size**: ~188KB (highly optimized native code)
- **Memory Usage**: ~10MB typical runtime
- **Startup Time**: <50ms cold start
- **Conversion Speed**: Native performance, limited only by FFmpeg
- **CPU Usage**: Minimal overhead, most work delegated to FFmpeg

## Troubleshooting

### Compiler Issues

**GCC/Clang Version**:

```bash
# Check compiler version
g++ --version
clang++ --version

# Install newer compiler (Ubuntu)
sudo apt update
sudo apt install gcc-9 g++-9

# Set as default
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90
```

**C++17 Support**:

```bash
# Test C++17 support
echo '#include <filesystem>' | g++ -x c++ -std=c++17 -c -
```

### Build Errors

**Missing CMake**:

```bash
# Install CMake (macOS)
brew install cmake

# Install CMake (Ubuntu)
sudo apt install cmake

# Verify installation
cmake --version
```

**Missing Build Tools**:

```bash
# macOS: Install Xcode command line tools
xcode-select --install

# Ubuntu: Install build essentials
sudo apt install build-essential

# Arch Linux: Install base development tools
sudo pacman -S base-devel
```

### Filesystem Library Issues

The project automatically detects and handles different filesystem library locations:

```cpp
// Modern C++17 (preferred)
#include <filesystem>
namespace fs = std::filesystem;

// Fallback for older systems
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
```

### Runtime Issues

**FFmpeg Not Found**:

```bash
# Test FFmpeg installation
ffmpeg -version

# Install FFmpeg (macOS)
brew install ffmpeg

# Install FFmpeg (Ubuntu)
sudo apt install ffmpeg
```

**Permission Denied**:

```bash
# Make scripts executable
chmod +x build.sh run.sh

# Or run binary directly
chmod +x build/bin/convert
```

## Development

### Building for Different Platforms

**Debug Build**:

```bash
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

**Cross-Compilation** (example for ARM):

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake ..
```

### Adding Features

The modular design makes it easy to extend:

#### New Video Formats

Edit `VideoFile::getSupportedExtensions()`:

```cpp
std::vector<std::string> VideoFile::getSupportedExtensions() {
    return {"mp4", "avi", "mov", "mkv", "flv", "wmv", "webm", "m4v", "3gp",
            "your_format"};  // Add here
}
```

#### Custom FFmpeg Parameters

Modify the command in `VideoConverter::convertToMp3()`:

```cpp
std::ostringstream cmdStream;
cmdStream << "ffmpeg -i \"" << inputPath << "\""
          << " -vn"                    // No video stream
          << " -acodec libmp3lame"     // MP3 encoder
          << " -ab 320k"               // Higher bitrate
          << " -ar 44100"              // Sample rate
          << " -y \"" << outputPath << "\"";
```

#### Additional Output Formats

Extend `getOutputPath()` to support multiple formats:

```cpp
std::string getOutputPath(const std::string& input, const std::string& format) {
    // Implementation for multiple output formats
}
```

### Code Quality

The project follows modern C++ best practices:

- **RAII**: Automatic resource management
- **Const Correctness**: Proper use of const throughout
- **Exception Safety**: Strong exception guarantees where possible
- **Modern Syntax**: Range-based loops, auto, etc.
- **Header Guards**: `#pragma once` for include protection

### Testing

Basic testing can be done with:

```bash
# Build and test basic functionality
./build.sh && echo "test" | ./build/bin/convert

# Memory leak testing with Valgrind (Linux)
valgrind --leak-check=full ./build/bin/convert

# Static analysis with Clang
clang-tidy src/*.cpp -- -std=c++17 -Iinclude
```

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Follow C++ Core Guidelines and project style
4. Ensure cross-platform compatibility
5. Add tests for new functionality
6. Run static analysis and memory checks
7. Commit changes (`git commit -m 'Add amazing feature'`)
8. Push to your branch (`git push origin feature/amazing-feature`)
9. Open a Pull Request

### Code Style Guidelines

- Follow C++ Core Guidelines
- Use `snake_case` for variables and functions
- Use `PascalCase` for classes and types
- Use `#pragma once` for header guards
- Include comprehensive error handling
- Document public APIs with Doxygen-style comments
- Prefer `std::` containers and algorithms
- Use RAII for resource management

## License

MIT License - see the project repository for details.
