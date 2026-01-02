@echo off
REM Qt WebEngine Installation Check Script for Windows
REM 檢查 Qt WebEngine 模組是否正確安裝

echo ==========================================
echo Qt WebEngine 安裝檢查工具 (Windows)
echo ==========================================
echo.

REM Check for qmake
where qmake6 >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    set QMAKE=qmake6
    echo [✓] 找到 qmake6
    goto :found_qmake
)

where qmake >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    set QMAKE=qmake
    echo [✓] 找到 qmake
    goto :found_qmake
)

echo [✗] 錯誤: 找不到 qmake
echo.
echo 請確保已安裝 Qt 並將其加入 PATH 環境變數
echo 或使用 Qt 提供的命令提示字元 (Qt Command Prompt)
echo.
pause
exit /b 1

:found_qmake

REM Get Qt installation path
for /f "tokens=*" %%i in ('%QMAKE% -query QT_INSTALL_LIBS') do set QT_INSTALL_LIBS=%%i
for /f "tokens=*" %%i in ('%QMAKE% -query QT_VERSION') do set QT_VERSION=%%i

echo   Qt 版本: %QT_VERSION%
echo.

echo 檢查 Qt WebEngine 模組...

REM Check for WebEngine DLL
set WEBENGINE_FOUND=0

if exist "%QT_INSTALL_LIBS%\Qt*WebEngine*.dll" (
    echo [✓] 找到 Qt WebEngine DLL
    set WEBENGINE_FOUND=1
)

if exist "%QT_INSTALL_LIBS%\Qt*WebEngine*.lib" (
    echo [✓] 找到 Qt WebEngine 函式庫
    set WEBENGINE_FOUND=1
)

echo.

if %WEBENGINE_FOUND% EQU 1 (
    echo ==========================================
    echo [✓] Qt WebEngine 已正確安裝！
    echo ==========================================
    echo.
    echo 您可以開始編譯專案：
    echo   使用 qmake:
    echo     %QMAKE% last-report.pro
    echo     nmake  或  mingw32-make
    echo.
    echo   使用 CMake:
    echo     mkdir build ^&^& cd build
    echo     cmake ..
    echo     cmake --build .
    echo.
    pause
    exit /b 0
) else (
    echo ==========================================
    echo [✗] 錯誤: Qt WebEngine 未安裝！
    echo ==========================================
    echo.
    echo 請使用以下方法之一安裝 Qt WebEngine：
    echo.
    echo 1. 使用 Qt Maintenance Tool:
    echo    - 執行 Qt Maintenance Tool
    echo    - 選擇 "Add or remove components"
    echo    - 展開您的 Qt 版本
    echo    - 勾選 "Qt WebEngine"
    echo    - 點擊 "Next" 完成安裝
    echo.
    echo 2. 重新安裝 Qt:
    echo    - 從 https://www.qt.io/download 下載 Qt
    echo    - 安裝時確保選擇 WebEngine 組件
    echo.
    echo 安裝完成後，請重新執行此腳本進行驗證。
    echo.
    pause
    exit /b 1
)
