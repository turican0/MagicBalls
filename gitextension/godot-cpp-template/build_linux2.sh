cd ../../

# Patch godot-cpp CMakeLists
GODOT_CPP_CMAKE="gitextension/godot-cpp-template/godot-cpp/CMakeLists.txt"
sed -i 's/-fno-exceptions/-fexceptions/g' "$GODOT_CPP_CMAKE"
sed -i 's/-fno-rtti/-frtti/g'             "$GODOT_CPP_CMAKE"
echo "=== Patched godot-cpp CMakeLists ==="
grep -n "exceptions\|rtti" "$GODOT_CPP_CMAKE" | head -20

# Extra flags pro GCC 13
EXTRA_FLAGS="-include atomic -include cmath"

# Release build
cmake -B build-cpp-release -S gitextension/godot-cpp-template \
  -DCMAKE_BUILD_TYPE=Release -DLIBNAME="MagicBalls" \
  -DGODOTCPP_DISABLE_EXCEPTIONS=OFF \
  -DCMAKE_CXX_FLAGS="$EXTRA_FLAGS" \
  -GNinja
ninja -C build-cpp-release

# Debug build
cmake -B build-cpp-debug -S gitextension/godot-cpp-template \
  -DCMAKE_BUILD_TYPE=Debug -DLIBNAME="MagicBalls" \
  -DGODOTCPP_DISABLE_EXCEPTIONS=OFF \
  -DCMAKE_CXX_FLAGS="$EXTRA_FLAGS" \
  -GNinja
ninja -C build-cpp-debug
