# Rust Video to Audio Converter

A Rust implementation of the interactive video-to-audio converter with modern async/await support, rich console UI, and excellent performance.

## Features

- ✅ Interactive directory and file selection with modern UI
- ✅ Support for multiple video formats (mp4, avi, mov, mkv, flv, wmv, webm, m4v, 3gp)
- ✅ Manual file path/URL input option
- ✅ Async/await based conversion with progress display
- ✅ Configuration via `.env` file
- ✅ Rich colored console output
- ✅ Fast compilation and execution
- ✅ Memory safe and performant
- ✅ Cross-platform compatibility

## Requirements

- **Rust 1.70+ (Edition 2021)** - Install from [rustup.rs](https://rustup.rs/)
- **FFmpeg** installed and available in system PATH

## Quick Start

### Option 1: Using the run script (recommended)

```bash
./run.sh
```

### Option 2: Build and run manually

```bash
# Build optimized release binary
./build.sh

# Run the binary directly
./target/release/convert
```

### Option 3: Using Cargo directly

```bash
# Run in development mode
cargo run

# Build and run optimized release
cargo build --release
./target/release/convert
```

## Build Process

The project uses Cargo with optimizations for small binary size:

- **LTO (Link Time Optimization)**: Enabled for maximum optimization
- **Single Codegen Unit**: Better optimization at cost of compile time
- **Strip Debug Info**: Removes debug symbols from release binary
- **Size Optimization**: Uses `opt-level = "s"` for smaller binaries

### Project Structure

```
rust/
├── src/
│   ├── main.rs                # Application entry point
│   ├── converter.rs          # Core conversion logic and async operations
│   ├── video_file.rs         # VideoFile struct with metadata
│   └── config.rs            # Configuration management (.env support)
├── target/release/convert    # Optimized binary (after build)
├── Cargo.toml               # Dependencies and build configuration
├── build.sh                 # Build script
├── run.sh                   # Run script
└── README.md                # This file
```

## Dependencies

The Rust version uses carefully selected crates for optimal functionality:

- **colored** (2.1): Rich colored console output
- **inquire** (0.7): Modern interactive prompts and menus
- **dotenv** (0.15): Environment variable loading from `.env` files
- **walkdir** (2.4): Efficient directory traversal
- **url** (2.5): URL parsing and validation
- **indicatif** (0.17): Progress bars and spinners
- **tokio** (1.39): Async runtime for non-blocking operations

## Architecture

### Async/Await Design

The Rust version leverages Tokio for:

- **Non-blocking FFmpeg execution**: Conversion doesn't freeze the UI
- **Concurrent stream processing**: Can read FFmpeg output while process runs
- **Graceful shutdown**: Proper signal handling (Ctrl+C)
- **Future-ready**: Easy to extend with additional async operations

### Memory Safety

Rust's ownership system provides:

- **Zero-cost abstractions**: No runtime overhead for safety
- **Memory safety**: No buffer overflows or memory leaks
- **Thread safety**: Safe concurrent operations when needed
- **Error handling**: Explicit error propagation with `Result<T, E>`

### Key Features

1. **Fast Startup**: Optimized binary with minimal dependencies
2. **Rich UI**: Colored output with emojis and progress indicators
3. **Cross-Platform**: Works on macOS, Linux, and Windows
4. **Async Operations**: Non-blocking FFmpeg execution
5. **Robust Error Handling**: Comprehensive error messages and recovery

## Usage Examples

### Interactive File Selection

1. Run the application: `./run.sh`
2. Confirm or choose working directory
3. Select from available video files using arrow keys
4. Watch real-time conversion progress
5. Find your MP3 in the same directory

### Manual File Input

- Select "Enter file path or URL manually" from the menu
- Provide full path to video file or HTTP/HTTPS URL
- Conversion proceeds with async progress display

### Configuration

Create a `.env` file in the parent directory:

```env
DEFAULT_DIR=/path/to/your/videos
```

## Performance

The Rust version offers excellent performance characteristics:

- **Binary Size**: ~980KB (highly optimized)
- **Memory Usage**: ~15MB typical runtime
- **Startup Time**: <100ms cold start
- **Conversion Speed**: Limited only by FFmpeg processing
- **CPU Usage**: Minimal overhead, most work done by FFmpeg

## Troubleshooting

### Rust Installation Issues

```bash
# Install Rust
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# Reload shell environment
source $HOME/.cargo/env

# Verify installation
cargo --version
```

### Build Errors

```bash
# Update Rust toolchain
rustup update

# Clean and rebuild
cargo clean
cargo build --release
```

### FFmpeg Not Found

```bash
# Test FFmpeg installation
ffmpeg -version

# Install FFmpeg (macOS)
brew install ffmpeg

# Install FFmpeg (Ubuntu/Debian)
sudo apt install ffmpeg
```

### Permission Denied

```bash
# Make scripts executable
chmod +x build.sh run.sh

# Or run binary directly
chmod +x target/release/convert
```

## Development

### Adding Features

The modular design makes it easy to extend:

#### New Video Formats

Edit the `SUPPORTED_EXTENSIONS` constant in `src/video_file.rs`:

```rust
const SUPPORTED_EXTENSIONS: &[&str] = &[
    "mp4", "avi", "mov", "mkv", "flv", "wmv", "webm", "m4v", "3gp",
    "your_format"  // Add here
];
```

#### Custom FFmpeg Parameters

Modify the args in `convert_to_mp3()` in `src/converter.rs`:

```rust
let mut cmd = Command::new("ffmpeg")
    .args(&[
        "-i", input_path,
        "-vn",                    // No video stream
        "-acodec", "libmp3lame",  // MP3 encoder
        "-ab", "320k",            // Higher bitrate
        "-ar", "44100",           // Sample rate
        "-y",                     // Overwrite output
        output_path,
    ])
    // ...
```

#### New Output Formats

Extend the `get_output_path()` method to support other formats:

```rust
fn get_output_path(&self, input_path: &str, format: &str) -> String {
    // Implementation for multiple output formats
}
```

### Code Quality

The project follows Rust best practices:

- **Error Handling**: Comprehensive `Result<T, E>` usage
- **Documentation**: Rustdoc comments for public APIs
- **Testing**: Unit tests for core functionality (expandable)
- **Linting**: Clippy-clean code
- **Formatting**: Standard `rustfmt` formatting

### Running Tests

```bash
# Run unit tests
cargo test

# Run with output
cargo test -- --nocapture

# Test specific module
cargo test config::tests
```

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Follow Rust conventions and run `cargo fmt`
4. Add tests for new functionality
5. Ensure all tests pass with `cargo test`
6. Run `cargo clippy` to check for issues
7. Commit changes (`git commit -m 'Add amazing feature'`)
8. Push to your branch (`git push origin feature/amazing-feature`)
9. Open a Pull Request

### Code Style Guidelines

- Follow standard Rust naming conventions (snake_case, PascalCase)
- Use `cargo fmt` for consistent formatting
- Add documentation comments for public APIs
- Handle all `Result` types explicitly
- Prefer `?` operator for error propagation
- Use meaningful variable and function names

## License

MIT License - see the project repository for details.
