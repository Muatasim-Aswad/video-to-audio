#!/bin/bash

# Run script for Rust Video to Audio Converter

BINARY_PATH="target/release/convert"

if [ ! -f "$BINARY_PATH" ]; then
    echo "📦 Binary not found. Building first..."
    ./build.sh
    
    if [ $? -ne 0 ]; then
        echo "❌ Build failed. Cannot run application."
        exit 1
    fi
fi

echo "🚀 Starting Rust Video to Audio Converter..."
"$BINARY_PATH"
