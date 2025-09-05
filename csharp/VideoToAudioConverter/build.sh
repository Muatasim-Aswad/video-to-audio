#!/bin/bash

# Build script for C# Video to Audio Converter
# Creates a self-contained executable for macOS

echo "🔨 Building C# Video to Audio Converter..."

# Clean previous builds
dotnet clean

# Build self-contained executable for macOS
dotnet publish -c Release -r osx-x64 --self-contained true -p:PublishSingleFile=true -p:PublishTrimmed=true

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "📦 Executable created at: bin/Release/net9.0/osx-x64/publish/VideoToAudioConverter"
    echo ""
    echo "To run the executable:"
    echo "./bin/Release/net9.0/osx-x64/publish/VideoToAudioConverter"
    echo ""
    echo "Or copy it to a directory in your PATH for global access."
else
    echo "❌ Build failed!"
    exit 1
fi
