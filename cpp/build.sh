#!/bin/bash

# Build script for C++ Video to Audio Converter
# Uses CMake to configure and build the project

echo "🔨 Building C++ Video to Audio Converter..."

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake not found. Please install CMake:"
    echo "  macOS: brew install cmake"
    echo "  Ubuntu: sudo apt install cmake"
    exit 1
fi

# Check if make is installed
if ! command -v make &> /dev/null; then
    echo "❌ Make not found. Please install build tools:"
    echo "  macOS: xcode-select --install"
    echo "  Ubuntu: sudo apt install build-essential"
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake (Release build for optimization)
echo "📝 Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=Release ..

if [ $? -ne 0 ]; then
    echo "❌ CMake configuration failed!"
    exit 1
fi

# Build the project
echo "🔧 Compiling C++ source code..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "📦 Executable created at: build/bin/convert"
    
    # Check binary size
    if [ -f "bin/convert" ]; then
        SIZE=$(ls -lh bin/convert | awk '{print $5}')
        echo "📊 Binary size: $SIZE"
    fi
    
    echo ""
    echo "To run the application:"
    echo "./build/bin/convert"
    echo ""
    echo "Or use the run script:"
    echo "./run.sh"
else
    echo "❌ Build failed!"
    exit 1
fi
