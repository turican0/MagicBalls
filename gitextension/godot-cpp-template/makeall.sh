#!/usr/bin/env bash
set -euo pipefail

CMAKE="../../../MagicBalls/tools/cmake-4.2.1-linux-x86_64/bin/cmake"
LIBNAME="EXTENSION-NAME"
GODOT_PROJECT_DIR="MBEXout"

# ================================================
# ANDROID NDK – adjust path if you have a different version
# ================================================
ANDROID_NDK_ROOT="${ANDROID_NDK_ROOT:-$HOME/Android/ndk/android-ndk-r27d}"

echo "[INFO] ANDROID_NDK_ROOT = $ANDROID_NDK_ROOT"
if [ ! -d "$ANDROID_NDK_ROOT" ]; then
    echo "[ERROR] NDK not found! Adjust the path above in this .sh file."
    exit 1
fi

echo "================================================"
echo " MBEX Godot GDExtension – BUILD ALL PLATFORMS"
echo "================================================"
echo

# ====================== LINUX ======================
echo "[1] Linux x86_64 → folder 'build'"
rm -rf "build"
"$CMAKE" -S . -B build -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  -DLIBNAME="$LIBNAME" -DGODOT_PROJECT_DIR="$GODOT_PROJECT_DIR"

echo "[1] Linux x86_32"
rm -rf "build-x86"
"$CMAKE" -S . -B build-x86 -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32" \
  -DLIBNAME="$LIBNAME" -DGODOT_PROJECT_DIR="$GODOT_PROJECT_DIR"

# ====================== ANDROID (Ninja – required for NDK) ======================
echo "[2] Android arm64"

# --- Android arm64 DEBUG ---
rm -rf "build-android-arm64-debug"
"$CMAKE" -S . -B build-android-arm64-debug -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 \
  -DGODOTCPP_DISABLE_EXCEPTIONS=OFF \
  -DLIBNAME="$LIBNAME" -DGODOT_PROJECT_DIR="$GODOT_PROJECT_DIR"

# --- Android arm64 RELEASE ---
rm -rf "build-android-arm64-release"
"$CMAKE" -S . -B build-android-arm64-release -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 \
  -DGODOTCPP_DISABLE_EXCEPTIONS=OFF \
  -DLIBNAME="$LIBNAME" -DGODOT_PROJECT_DIR="$GODOT_PROJECT_DIR"

echo "[2] Android x86_64"
rm -rf "build-android-x86_64"
"$CMAKE" -S . -B build-android-x86_64 -G "Ninja" \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=x86_64 -DANDROID_PLATFORM=android-24 \
  -DLIBNAME="$LIBNAME" -DGODOT_PROJECT_DIR="$GODOT_PROJECT_DIR"

# ====================== WEB ======================
if command -v emcmake &> /dev/null; then
    echo "[3] Web (wasm32)"
    rm -rf "build-web"
    emcmake "$CMAKE" -S . -B build-web -DCMAKE_BUILD_TYPE=Release \
      -DLIBNAME="$LIBNAME" -DGODOT_PROJECT_DIR="$GODOT_PROJECT_DIR"
else
    echo "[3] WEB skipped (run: source /path/to/emsdk/emsdk_env.sh)"
fi

echo
echo "=== Done! ==="
echo
echo "Android folders ready:"
echo "  build-android-arm64-debug"
echo "  build-android-arm64-release"
echo "  build-android-x86_64"
echo
echo "How to build Android Release:"
echo "  cd build-android-arm64-release"
echo "  cmake --build . --parallel"
echo
echo "  cd build-android-x86_64"
echo "  cmake --build . --parallel"
echo
echo "Linux build:"
echo "  cd build && cmake --build . --parallel"
