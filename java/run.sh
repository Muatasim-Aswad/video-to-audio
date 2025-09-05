#!/bin/bash

# Run script for Java Video to Audio Converter

JAR_FILE="video-to-audio-converter.jar"

if [ ! -f "$JAR_FILE" ]; then
    echo "📦 JAR file not found. Building first..."
    ./build.sh
    
    if [ $? -ne 0 ]; then
        echo "❌ Build failed. Cannot run application."
        exit 1
    fi
fi

echo "🚀 Starting Java Video to Audio Converter..."
java -jar "$JAR_FILE"
