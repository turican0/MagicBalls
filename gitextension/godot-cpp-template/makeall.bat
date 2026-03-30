@echo off
setlocal enabledelayedexpansion

set CMAKE=..\..\..\MagicBalls\tools\cmake-4.2.1-windows-x86_64\bin\cmake
set LIBNAME=EXTENSION-NAME
set GODOT_PROJECT_DIR=MBEXout

:: ================================================
:: ANDROID NDK – uprav cestu, pokud máš jinou verzi
:: ================================================
set ANDROID_NDK_ROOT=C:\Android\ndk\android-ndk-r27d
echo [INFO] ANDROID_NDK_ROOT = %ANDROID_NDK_ROOT%
if not exist "%ANDROID_NDK_ROOT%" (
    echo [ERROR] NDK nenalezen! Uprav cestu výše v tomto .bat souboru.
    pause
    exit /b 1
)

echo ================================================
echo  MBEX Godot GDExtension – BUILD ALL PLATFORMS
echo ================================================
echo.

:: ====================== WINDOWS ======================
echo [1] Windows x86_64 → složka "build"
rmdir /s /q "build" >nul 2>&1
%CMAKE% -S . -B build -G "Visual Studio 17 2022" -A x64 ^
  -DLIBNAME=%LIBNAME% -DGODOT_PROJECT_DIR=%GODOT_PROJECT_DIR%

echo [1] Windows x86_32
rmdir /s /q "build-x86" >nul 2>&1
%CMAKE% -S . -B build-x86 -G "Visual Studio 17 2022" -A Win32 ^
  -DLIBNAME=%LIBNAME% -DGODOT_PROJECT_DIR=%GODOT_PROJECT_DIR%

:: ====================== ANDROID (používá Ninja - nutné pro NDK) ======================
echo [2] Android arm64
:: --- Android arm64 DEBUG ---
rmdir /s /q "build-android-arm64-debug" >nul 2>&1
%CMAKE% -S . -B build-android-arm64-debug -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Debug ^
  -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK_ROOT%/build/cmake/android.toolchain.cmake" ^
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 ^
  -DGODOTCPP_DISABLE_EXCEPTIONS=OFF ^
  -DLIBNAME=%LIBNAME% -DGODOT_PROJECT_DIR=%GODOT_PROJECT_DIR%

:: --- Android arm64 RELEASE ---
rmdir /s /q "build-android-arm64-release" >nul 2>&1
%CMAKE% -S . -B build-android-arm64-release -G "Ninja" ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK_ROOT%/build/cmake/android.toolchain.cmake" ^
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 ^
  -DGODOTCPP_DISABLE_EXCEPTIONS=OFF ^
  -DLIBNAME=%LIBNAME% -DGODOT_PROJECT_DIR=%GODOT_PROJECT_DIR%

echo [2] Android x86_64
rmdir /s /q "build-android-x86_64" >nul 2>&1
%CMAKE% -S . -B build-android-x86_64 -G "Ninja" ^
  -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK_ROOT%/build/cmake/android.toolchain.cmake" ^
  -DANDROID_ABI=x86_64 -DANDROID_PLATFORM=android-24 ^
  -DLIBNAME=%LIBNAME% -DGODOT_PROJECT_DIR=%GODOT_PROJECT_DIR%

:: ====================== WEB ======================
where emcmake >nul 2>&1
if %errorlevel%==0 (
    echo [3] Web (wasm32)
    rmdir /s /q "build-web" >nul 2>&1
    emcmake %CMAKE% -S . -B build-web -DCMAKE_BUILD_TYPE=Release ^
      -DLIBNAME=%LIBNAME% -DGODOT_PROJECT_DIR=%GODOT_PROJECT_DIR%
) else (
    echo [3] WEB přeskočeno (spusť emsdk_env.bat)
)

echo.
echo === Hotovo! ===
echo.
echo Android složky připraveny:
echo   build-android-arm64
echo   build-android-x86_64
echo.
echo Jak sestavit Android Release:
echo   cd build-android-arm64
echo   cmake --build . --config Release --parallel
echo.
echo   cd build-android-x86_64
echo   cmake --build . --config Release --parallel
echo.
echo Windows zůstává stejný: otevři build\godot-cpp-template.sln

endlocal