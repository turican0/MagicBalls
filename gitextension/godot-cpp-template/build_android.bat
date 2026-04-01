cd build-android-arm64-debug
cmake --build . --config Debug --parallel
cd ..
cd build-android-arm64-release
cmake --build . --config Release --parallel
cd ..