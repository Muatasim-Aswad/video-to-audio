#!/bin/bash

# Build script for Rust Video to Audio Converter
# Creates optimized binary with all dependencies included

echo "🔨 Building Rust Video to Audio Converter..."

# Check if Rust is installed
if ! command -v cargo &> /dev/null; then
    echo "❌ Cargo/Rust not found. Please install Rust from https://rustup.rs/"
    exit 1
fi

echo "📝 Compiling Rust source code..."

# Build in release mode for optimized binary
cargo build --release

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "📦 Optimized binary created at: target/release/convert"
    
    # Check binary size
    if [ -f "target/release/convert" ]; then
        SIZE=$(ls -lh target/release/convert | awk '{print $5}')
        echo "📊 Binary size: $SIZE"
    fi
    
    echo ""
    echo "To run the application:"
    echo "./target/release/convert"
    echo ""
    echo "Or use the run script:"
    echo "./run.sh"
else
    echo "❌ Build failed!"
    exit 1
fi
