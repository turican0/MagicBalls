#!/bin/bash
set -e

LIBNAME="EXTENSION-NAME"
GODOT_PROJECT_DIR="../../godot-code/MBEXout"

echo "========================================"
echo "MBEX Godot GDExtension – BUILD LINUX"
echo "========================================"

# x86_64
echo "[1] Linux x86_64"
rm -rf build-linux-x86_64
cmake -S . -B build-linux-x86_64 -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  -DLIBNAME=${LIBNAME} -DGODOT_PROJECT_DIR=${GODOT_PROJECT_DIR}

# arm64
echo "[2] Linux arm64"
rm -rf build-linux-arm64
cmake -S . -B build-linux-arm64 -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DLIBNAME=${LIBNAME} -DGODOT_PROJECT_DIR=${GODOT_PROJECT_DIR}

# rv64 (pokud potřebuješ)
echo "[3] Linux rv64 (riscv64)"
rm -rf build-linux-rv64
cmake -S . -B build-linux-rv64 -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES=riscv64 \
  -DLIBNAME=${LIBNAME} -DGODOT_PROJECT_DIR=${GODOT_PROJECT_DIR}

echo "Hotovo! Teď sestav:"
echo "  cd build-linux-x86_64 && ninja"
echo "  cd build-linux-arm64 && ninja"
echo "  cd build-linux-rv64 && ninja"