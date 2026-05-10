setlocal EnableDelayedExpansion

:: ============================================================
:: KONFIGURACE - uprav NDK_PATH podle sveho prostredi
:: ============================================================
set NDK_PATH=C:\adb\ndk\27.2.12479018
set LIBNAME=MagicBalls
set SOURCE_DIR=.

:: Overeni ze NDK existuje
set TOOLCHAIN=%NDK_PATH%\build\cmake\android.toolchain.cmake
if not exist "%TOOLCHAIN%" (
    echo CHYBA: NDK toolchain nenalezen: %TOOLCHAIN%
    echo Zkontroluj NDK_PATH na zacatku tohoto skriptu.
    exit /b 1
)
echo Pouzivam NDK: %NDK_PATH%

:: Auto-detekce Ninja (hleda v PATH, CMake, NDK)
set GENERATOR=Ninja
where ninja >nul 2>&1
if errorlevel 1 (
    if exist "%NDK_PATH%\prebuilt\windows-x86_64\bin\ninja.exe" (
        set "PATH=%NDK_PATH%\prebuilt\windows-x86_64\bin;%PATH%"
        echo Pouzivam Ninja z NDK.
    ) else (
        echo Ninja nenalezena, pouzivam NMake...
        set GENERATOR=NMake Makefiles
    )
)

:: ============================================================
:: DEBUG build (arm64-v8a)
:: ============================================================
echo.
echo [1/4] Konfiguruji Debug build (arm64-v8a)...
if not exist build-android-arm64-debug mkdir build-android-arm64-debug

cmake -B build-android-arm64-debug ^
    -S "%SOURCE_DIR%" ^
    -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%" ^
    -DANDROID_ABI=arm64-v8a ^
    -DANDROID_PLATFORM=android-24 ^
    -DGODOTCPP_DISABLE_EXCEPTIONS=OFF ^
    -DGODOTCPP_TARGET=template_debug ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DLIBNAME="%LIBNAME%" ^
    -G "%GENERATOR%"
if errorlevel 1 ( echo CHYBA: configure Debug selhal! & exit /b 1 )

echo.
echo [2/4] Stavim Debug build...
cmake --build build-android-arm64-debug --config Debug --parallel
if errorlevel 1 ( echo CHYBA: build Debug selhal! & exit /b 1 )

:: ============================================================
:: RELEASE build (arm64-v8a)
:: ============================================================
echo.
echo [3/4] Konfiguruji Release build (arm64-v8a)...
if not exist build-android-arm64-release mkdir build-android-arm64-release

cmake -B build-android-arm64-release ^
    -S %SOURCE_DIR% ^
    -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%" ^
    -DANDROID_ABI=arm64-v8a ^
    -DANDROID_PLATFORM=android-24 ^
    -DGODOTCPP_DISABLE_EXCEPTIONS=OFF ^
    -DGODOTCPP_TARGET=template_release ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DLIBNAME="%LIBNAME%" ^
    -G "%GENERATOR%"
if errorlevel 1 ( echo CHYBA: configure Release selhal! & exit /b 1 )

echo.
echo [4/4] Stavim Release build...
cmake --build build-android-arm64-release --config Release --parallel
if errorlevel 1 ( echo CHYBA: build Release selhal! & exit /b 1 )

:: ============================================================
:: HOTOVO
:: ============================================================
echo.
echo ============================================================
echo  BUILD DOKONCEN
echo ============================================================
echo.
echo  Debug .so:
echo    godot-code\MBEXout\bin\android\libEXTENSION-NAME.android.template_debug.arm64.so
echo.
echo  Release .so:
echo    godot-code\MBEXout\bin\android.arm64-v8a\lib%LIBNAME%.android.arm64-v8a.template_release.arm64.so
echo.
echo  Logy z telefonu:  adb logcat -s godot
echo  Crash symbolizace: adb logcat | "%NDK_PATH%\ndk-stack" -sym godot-code\MBEXout\bin\android
echo.

endlocal