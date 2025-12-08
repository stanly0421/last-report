@echo off
REM YouTube Music Player - API Key Setup Script (Windows)
REM This script helps you configure your YouTube API key

echo =========================================
echo YouTube Music Player - API Key Setup
echo =========================================
echo.

REM Check if widget.cpp exists
if not exist "widget.cpp" (
    echo Error: widget.cpp not found!
    echo Please run this script from the project root directory.
    pause
    exit /b 1
)

echo Please follow these steps to get your YouTube API Key:
echo.
echo 1. Go to: https://console.cloud.google.com/
echo 2. Create a new project or select an existing one
echo 3. Enable 'YouTube Data API v3'
echo 4. Create credentials (API Key)
echo 5. Copy your API key
echo.
echo For detailed instructions, see USER_GUIDE.md
echo.

REM Prompt for API key
set /p api_key="Enter your YouTube API Key: "

if "%api_key%"=="" (
    echo Error: API Key cannot be empty!
    pause
    exit /b 1
)

REM Backup original file
copy widget.cpp widget.cpp.backup >nul

REM Replace the API key in widget.cpp using PowerShell with a more specific pattern
powershell -Command "(Get-Content widget.cpp) -replace 'apiKey\(\"YOUR_YOUTUBE_API_KEY_HERE\"\)', 'apiKey(\"%api_key%\")' | Set-Content widget.cpp"

if %errorlevel% equ 0 (
    echo.
    echo SUCCESS: API Key has been configured successfully!
    echo SUCCESS: Original file backed up to widget.cpp.backup
    echo.
    echo Next steps:
    echo 1. Open the project in Qt Creator
    echo 2. Build and run the application
    echo.
) else (
    echo Error: Failed to update widget.cpp
    REM Restore backup
    copy widget.cpp.backup widget.cpp >nul
    pause
    exit /b 1
)

pause
