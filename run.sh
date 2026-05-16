#!/bin/bash
echo "=== Starting Jets Bot ==="

BINARY="./build/bin/Jets_Bot"

if [ -f "$BINARY" ]; then
    sudo $BINARY
else
    echo "❌ Binary not found! Run ./build.sh first."
    exit 1
fi