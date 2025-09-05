#!/bin/bash

# Build script for Java Video to Audio Converter
# Simple compilation using javac (no Maven required)

echo "🔨 Building Java Video to Audio Converter..."

# Create build directory
mkdir -p build/classes

# Compile Java files
echo "📝 Compiling Java source files..."
javac -d build/classes -sourcepath src/main/java \
    src/main/java/com/hackyourfuture/videoconverter/*.java

if [ $? -ne 0 ]; then
    echo "❌ Compilation failed!"
    exit 1
fi

# Create JAR file
echo "📦 Creating JAR file..."
cd build/classes
jar cfe ../../video-to-audio-converter.jar com.hackyourfuture.videoconverter.VideoConverterApp \
    com/hackyourfuture/videoconverter/*.class

if [ $? -eq 0 ]; then
    cd ../..
    echo "✅ Build successful!"
    echo "📦 JAR created: video-to-audio-converter.jar"
    echo ""
    echo "To run the application:"
    echo "java -jar video-to-audio-converter.jar"
    echo ""
    echo "Or use the run script:"
    echo "./run.sh"
else
    echo "❌ JAR creation failed!"
    exit 1
fi
