rmdir /s /q "build"
..\..\..\MagicBalls\tools\cmake-4.2.1-windows-x86_64\bin\cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DGODOT_PROJECT_DIR=..\..\godot-code\MBEXout