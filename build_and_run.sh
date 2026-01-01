#!/bin/bash

echo "========================================"
echo "MYOS Build and Run Script"
echo "========================================"
echo ""

echo "[1/3] Cleaning build directory..."
make clean
if [ $? -ne 0 ]; then
    echo "ERROR: Clean failed!"
    exit 1
fi
echo "Clean completed."
echo ""

echo "[2/3] Building kernel..."
make
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed!"
    exit 1
fi
echo "Build completed."
echo ""

echo "[3/3] Running QEMU..."
echo "Press Ctrl+C to stop QEMU"
echo ""
make run
if [ $? -ne 0 ]; then
    echo "ERROR: Run failed!"
    exit 1
fi