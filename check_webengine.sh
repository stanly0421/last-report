#!/bin/bash

# Qt WebEngine Installation Check Script
# 檢查 Qt WebEngine 模組是否正確安裝

echo "=========================================="
echo "Qt WebEngine 安裝檢查工具"
echo "=========================================="
echo ""

# Check for qmake
QMAKE=""
if command -v qmake6 &> /dev/null; then
    QMAKE="qmake6"
    echo "✓ 找到 qmake6"
elif command -v qmake-qt6 &> /dev/null; then
    QMAKE="qmake-qt6"
    echo "✓ 找到 qmake-qt6"
elif command -v qmake &> /dev/null; then
    QMAKE="qmake"
    echo "✓ 找到 qmake"
else
    echo "✗ 錯誤: 找不到 qmake"
    echo ""
    echo "請先安裝 Qt:"
    echo "  Ubuntu/Debian: sudo apt-get install qt6-base-dev"
    echo "  Fedora/RHEL:   sudo dnf install qt6-qtbase-devel"
    echo "  macOS:         brew install qt@6"
    exit 1
fi

# Get Qt version
QT_VERSION=$($QMAKE -query QT_VERSION 2>/dev/null)
QT_INSTALL_LIBS=$($QMAKE -query QT_INSTALL_LIBS 2>/dev/null)
QT_INSTALL_HEADERS=$($QMAKE -query QT_INSTALL_HEADERS 2>/dev/null)

echo "  Qt 版本: $QT_VERSION"
echo ""

# Check for WebEngine library
echo "檢查 Qt WebEngine 模組..."
WEBENGINE_FOUND=0

if [ -d "$QT_INSTALL_LIBS" ]; then
    if ls "$QT_INSTALL_LIBS"/libQt*WebEngine* &> /dev/null || \
       ls "$QT_INSTALL_LIBS"/Qt*WebEngine*.so* &> /dev/null || \
       ls "$QT_INSTALL_LIBS"/QtWebEngine*.framework &> /dev/null 2>&1; then
        echo "✓ 找到 Qt WebEngine 函式庫"
        WEBENGINE_FOUND=1
    fi
fi

# Check for WebEngine headers
if [ -d "$QT_INSTALL_HEADERS/QtWebEngineWidgets" ] || \
   [ -d "$QT_INSTALL_HEADERS/QtWebEngine" ]; then
    echo "✓ 找到 Qt WebEngine 標頭檔"
    WEBENGINE_FOUND=1
fi

echo ""

if [ $WEBENGINE_FOUND -eq 1 ]; then
    echo "=========================================="
    echo "✓ Qt WebEngine 已正確安裝！"
    echo "=========================================="
    echo ""
    echo "您可以開始編譯專案："
    echo "  使用 qmake:"
    echo "    $QMAKE last-report.pro"
    echo "    make"
    echo ""
    echo "  使用 CMake:"
    echo "    mkdir build && cd build"
    echo "    cmake .."
    echo "    cmake --build ."
    exit 0
else
    echo "=========================================="
    echo "✗ 錯誤: Qt WebEngine 未安裝！"
    echo "=========================================="
    echo ""
    echo "請安裝 Qt WebEngine 模組："
    echo ""
    
    # Detect OS and provide specific instructions
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        case "$ID" in
            ubuntu|debian)
                QT_MAJOR=$(echo $QT_VERSION | cut -d. -f1)
                echo "Ubuntu/Debian:"
                echo "  sudo apt-get update"
                echo "  sudo apt-get install qt${QT_MAJOR}-webengine-dev"
                ;;
            fedora|rhel|centos)
                QT_MAJOR=$(echo $QT_VERSION | cut -d. -f1)
                echo "Fedora/RHEL/CentOS:"
                echo "  sudo dnf install qt${QT_MAJOR}-qtwebengine-devel"
                ;;
            *)
                echo "請參考 README.md 中的安裝指南"
                ;;
        esac
    elif [ "$(uname)" == "Darwin" ]; then
        echo "macOS:"
        echo "  brew install qt@6"
        echo "  或使用 Qt Maintenance Tool 安裝 WebEngine 組件"
    else
        echo "請參考 README.md 中的安裝指南"
    fi
    
    echo ""
    echo "安裝完成後，請重新執行此腳本進行驗證。"
    exit 1
fi
