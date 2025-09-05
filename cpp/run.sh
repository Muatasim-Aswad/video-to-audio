#!/bin/bash

# Run script for C++ Video to Audio Converter

BINARY_PATH="build/bin/convert"

if [ ! -f "$BINARY_PATH" ]; then
    echo "📦 Binary not found. Building first..."
    ./build.sh
    
    if [ $? -ne 0 ]; then
        echo "❌ Build failed. Cannot run application."
        exit 1
    fi
fi

echo "🚀 Starting C++ Video to Audio Converter..."
"$BINARY_PATH"
