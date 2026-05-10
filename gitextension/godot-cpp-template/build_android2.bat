setlocal EnableDelayedExpansion

:: ============================================================
:: KONFIGURACE - uprav NDK_PATH podle sveho prostredi
:: ============================================================
set NDK_PATH=C:\adb\ndk\27.2.12479018
set LIBNAME=MagicBalls
set SOURCE_DIR=.
set BIN_DIR=godot-code\MBEXout\bin

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

:: ============================================================
:: 1. DEBUG build (arm64-v8a)
:: ============================================================
echo.
echo [1/4] Konfiguruji Debug build (arm64-v8a)...
cmake -B build-android-arm64-debug ^
    -S "%SOURCE_DIR%" ^
    -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%" ^
    -DANDROID_ABI=arm64-v8a ^
    -DANDROID_PLATFORM=android-24 ^
    -DGODOTCPP_TARGET=template_debug ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DLIBNAME="%LIBNAME%" ^
    -G "%GENERATOR%"

echo [2/4] Stavim Debug build...
cmake --build build-android-arm64-debug --config Debug --parallel

:: --- MANIPULACE S DEBUG SOUBOREM ---
if not exist "%BIN_DIR%\android" mkdir "%BIN_DIR%\android"
:: Najdeme soubor (at uz se jmenuje jakkoliv) a vnutime mu tvuj nazev
if exist "%BIN_DIR%\android.arm64-v8a\MagicBalls.android.arm64-v8a.template_debug.arm64.debug.so" (
    move /Y "%BIN_DIR%\android.arm64-v8a\MagicBalls.android.arm64-v8a.template_debug.arm64.debug.so" "%BIN_DIR%\android.arm64-v8a\libEXTENSION-NAME.android.template_debug.arm64.so"
)

:: ============================================================
:: 2. RELEASE build (arm64-v8a)
:: ============================================================
echo.
echo [3/4] Konfiguruji Release build (arm64-v8a)...
cmake -B build-android-arm64-release ^
    -S "%SOURCE_DIR%" ^
    -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%" ^
    -DANDROID_ABI=arm64-v8a ^
    -DANDROID_PLATFORM=android-24 ^
    -DGODOTCPP_TARGET=template_release ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DLIBNAME="%LIBNAME%" ^
    -G "%GENERATOR%"

echo [4/4] Stavim Release build...
cmake --build build-android-arm64-release --config Release --parallel

:: --- MANIPULACE S RELEASE SOUBOREM ---
if not exist "%BIN_DIR%\android.arm64-v8a" mkdir "%BIN_DIR%\android.arm64-v8a"
:: CMake to pravdepodobne hodi do /android/, tak to presuneme do /android.arm64-v8a/ a prejmenujeme
if exist "%BIN_DIR%\android\lib%LIBNAME%.android.template_release.arm64.so" (
    move /Y "%BIN_DIR%\android\lib%LIBNAME%.android.template_release.arm64.so" "%BIN_DIR%\android.arm64-v8a\lib%LIBNAME%.android.arm64-v8a.template_release.arm64.so"
)

:: ============================================================
:: FINÁLNÍ VÝPIS
:: ============================================================
echo.
echo ============================================================
echo  HOTOVO - Soubory jsou pripraveny v techto cestach:
echo ============================================================
echo.
echo android.arm64.single.debug = "res://MBEXout/bin/android/libEXTENSION-NAME.android.template_debug.arm64.so"
echo android.arm64.double.debug = "res://MBEXout/bin/android/libEXTENSION-NAME.android.template_debug.arm64.so"
echo.
echo android.arm64.single.release = "res://MBEXout/bin/android.arm64-v8a/lib%LIBNAME%.android.arm64-v8a.template_release.arm64.so"
echo android.arm64.double.release = "res://MBEXout/bin/android.arm64-v8a/lib%LIBNAME%.android.arm64-v8a.template_release.arm64.so"
echo.

endlocal