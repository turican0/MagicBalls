@echo off

:: ============================================================
:: KONFIGURACE - uprav NDK_PATH podle sveho prostredi
:: ============================================================
set NDK_PATH=C:\adb\ndk\27.2.12479018
set LIBNAME=MagicBalls
set SOURCE_DIR=.

:: Relativni cesta od mista spusteni skriptu
set ABI_DIR=..\..\godot-code\MBEXout\bin\android.arm64-v8a

:: Overeni ze NDK existuje
set TOOLCHAIN=%NDK_PATH%\build\cmake\android.toolchain.cmake
if not exist "%TOOLCHAIN%" (
    echo CHYBA: NDK toolchain nenalezen: %TOOLCHAIN%
    exit /b 1
)

:: Auto-detekce Ninja
set GENERATOR=Ninja
where ninja >nul 2>&1
if errorlevel 1 (
    if exist "%NDK_PATH%\prebuilt\windows-x86_64\bin\ninja.exe" (
        set "PATH=%NDK_PATH%\prebuilt\windows-x86_64\bin;%PATH%"
    ) else (
        set GENERATOR=NMake Makefiles
    )
)

if not exist "%ABI_DIR%" mkdir "%ABI_DIR%"

:: ============================================================
:: 1. DEBUG build (arm64-v8a)
:: ============================================================
echo.
echo [1/4] Konfiguruji Debug build (arm64-v8a)...
cmake -B build-android-arm64-debug -S "%SOURCE_DIR%" -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%" -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 -DGODOTCPP_TARGET=template_debug -DCMAKE_BUILD_TYPE=Debug -DLIBNAME="%LIBNAME%" -DANDROID=1 -G "%GENERATOR%"
if errorlevel 1 ( echo CHYBA: cmake konfigurace debug selhala & exit /b 1 )

echo [2/4] Stavim Debug build...
cmake --build build-android-arm64-debug --config Debug --parallel
if errorlevel 1 ( echo CHYBA: cmake build debug selhal & exit /b 1 )

:: Zkontroluj ze zdrojovy soubor existuje
if not exist "%ABI_DIR%\MagicBalls.android.arm64-v8a.template_debug.arm64.debug.so" (
    echo CHYBA: Nenalezen zdrojovy debug soubor!
    echo Hledam v: %ABI_DIR%
    dir "%ABI_DIR%"
    exit /b 1
)

:: Pevne zakodovany copy - relativni cesty
copy /Y "..\..\godot-code\MBEXout\bin\android.arm64-v8a\MagicBalls.android.arm64-v8a.template_debug.arm64.debug.so" "..\..\godot-code\MBEXout\bin\android.arm64-v8a\libEXTENSION-NAME.android.template_debug.arm64.so"
echo OK: APK/gdextension -> libEXTENSION-NAME.android.template_debug.arm64.so

copy /Y "..\..\godot-code\MBEXout\bin\android.arm64-v8a\MagicBalls.android.arm64-v8a.template_debug.arm64.debug.so" "..\..\godot-code\MBEXout\bin\android.arm64-v8a\libEXTENSION-NAME.android.template_debug.arm64.debug.so"
echo OK: Android Studio  -> libEXTENSION-NAME.android.template_debug.arm64.debug.so

:: ============================================================
:: 2. RELEASE build (arm64-v8a)
:: ============================================================
echo.
echo [3/4] Konfiguruji Release build (arm64-v8a)...
cmake -B build-android-arm64-release -S "%SOURCE_DIR%" -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%" -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24 -DGODOTCPP_TARGET=template_release -DCMAKE_BUILD_TYPE=Release -DLIBNAME="%LIBNAME%" -G "%GENERATOR%"
if errorlevel 1 ( echo CHYBA: cmake konfigurace release selhala & exit /b 1 )

echo [4/4] Stavim Release build...
cmake --build build-android-arm64-release --config Release --parallel
if errorlevel 1 ( echo CHYBA: cmake build release selhal & exit /b 1 )

:: ============================================================
:: FINALNI VYPIS
:: ============================================================
echo.
echo ============================================================
echo  HOTOVO
echo ============================================================
echo.
echo Soubory pro .gdextension:
echo   android.arm64.*.debug   = res://MBEXout/bin/android.arm64-v8a/libEXTENSION-NAME.android.template_debug.arm64.so
echo   android.arm64.*.release = res://MBEXout/bin/android.arm64-v8a/MagicBalls.android.arm64-v8a.template_release.arm64.so
echo.
echo Soubor pro Android Studio (debug symboly):
echo   %ABI_DIR%\libEXTENSION-NAME.android.template_debug.arm64.debug.so
echo.