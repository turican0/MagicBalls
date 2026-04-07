@echo off
set "targetDir=%APPDATA%\Godot\app_userdata\MagicBalls"

echo.

if exist "%targetDir%" (
    echo Deleting folder: %targetDir%
    rd /s /q "%targetDir%"
    echo Folder deleted successfully.
) else (
    echo Folder not found.
)

pause