#!/bin/bash
set -e

LIBNAME="EXTENSION-NAME"
GODOT_PROJECT_DIR="../../godot-code/MBEXout"

echo "========================================"
echo "MBEX Godot GDExtension – BUILD macOS + iOS"
echo "========================================"

# macOS (universal – arm64 + x86_64)
echo "[1] macOS (arm64 + x86_64)"
rm -rf build-macos
cmake -S . -B build-macos -G "Xcode" \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
  -DLIBNAME=${LIBNAME} -DGODOT_PROJECT_DIR=${GODOT_PROJECT_DIR}

# iOS (arm64)
echo "[2] iOS arm64"
rm -rf build-ios
cmake -S . -B build-ios -G "Xcode" \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 \
  -DLIBNAME=${LIBNAME} -DGODOT_PROJECT_DIR=${GODOT_PROJECT_DIR}

echo "Hotovo! Teď otevři Xcode projekty a sestav:"
echo "  open build-macos/godot-cpp-template.xcodeproj"
echo "  open build-ios/godot-cpp-template.xcodeproj"
echo ""
echo "Nebo příkazově:"
echo "  cd build-macos && xcodebuild -configuration Release -target ${LIBNAME}"
echo "  cd build-ios && xcodebuild -configuration Release -target ${LIBNAME}"