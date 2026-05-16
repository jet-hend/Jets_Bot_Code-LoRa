#!/bin/bash
echo "=== Jets Bot Build Script ==="

# Clean previous build if requested
if [ "$1" = "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build/
fi

mkdir -p build
cd build

echo "Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

echo "Building..."
cmake --build . -j$(nproc)

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "Binary is at: build/bin/Jets_Bot"
else
    echo "❌ Build failed!"
fi