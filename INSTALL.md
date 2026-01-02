# 安裝指南 (Installation Guide)

本文件提供詳細的安裝和疑難排解指南，特別針對常見的 Qt WebEngine 安裝問題。

## 目錄
- [快速開始](#快速開始)
- [詳細安裝步驟](#詳細安裝步驟)
- [常見問題](#常見問題)
- [疑難排解](#疑難排解)

## 快速開始

### 1. 檢查 Qt WebEngine 是否已安裝

執行檢查腳本：

**Linux/macOS:**
```bash
./check_webengine.sh
```

**Windows:**
```cmd
check_webengine.bat
```

如果顯示 "✓ Qt WebEngine 已正確安裝"，您可以直接跳到[編譯步驟](#編譯專案)。

### 2. 安裝 Qt 和 WebEngine

如果檢查失敗，請按照下方對應的作業系統說明安裝。

## 詳細安裝步驟

### Ubuntu/Debian

```bash
# 更新套件列表
sudo apt-get update

# 安裝 Qt 6 和所有需要的模組
sudo apt-get install -y \
    qt6-base-dev \
    qt6-webengine-dev \
    qt6-multimedia-dev \
    libqt6webenginewidgets6

# 驗證安裝
./check_webengine.sh
```

如果使用 Qt 5：
```bash
sudo apt-get install -y \
    qtbase5-dev \
    qtwebengine5-dev \
    qtmultimedia5-dev \
    libqt5webenginewidgets5
```

### Fedora/RHEL/CentOS

```bash
# Qt 6
sudo dnf install -y \
    qt6-qtbase-devel \
    qt6-qtwebengine-devel \
    qt6-qtmultimedia-devel

# 或 Qt 5
sudo dnf install -y \
    qt5-qtbase-devel \
    qt5-qtwebengine-devel \
    qt5-qtmultimedia-devel
```

### macOS

#### 使用 Homebrew (推薦)

```bash
# 安裝 Qt 6
brew install qt@6

# 將 Qt 加入 PATH
echo 'export PATH="/usr/local/opt/qt@6/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc

# 驗證安裝
./check_webengine.sh
```

#### 使用 Qt 官方安裝程式

1. 下載 Qt 線上安裝程式：https://www.qt.io/download
2. 執行安裝程式
3. 選擇 Qt 版本時，確保勾選：
   - Qt WebEngine
   - Qt Multimedia
4. 完成安裝

### Windows

#### 方法 1: 使用 Qt Maintenance Tool (推薦)

如果您已經安裝了 Qt 但缺少 WebEngine：

1. 找到並執行 **Qt Maintenance Tool**
   - 通常位於 `C:\Qt\MaintenanceTool.exe`
2. 選擇 **"Add or remove components"**
3. 點擊 **"Next"**
4. 展開您的 Qt 版本 (例如 Qt 6.5.0)
5. 勾選以下組件：
   - ☑ **Qt WebEngine**
   - ☑ Qt Multimedia (如果未安裝)
   - ☑ Qt Network (如果未安裝)
6. 點擊 **"Next"** 並完成安裝
7. 執行 `check_webengine.bat` 驗證

#### 方法 2: 全新安裝 Qt

1. 下載 Qt 線上安裝程式：
   - https://www.qt.io/download-qt-installer
2. 執行安裝程式
3. 登入或創建 Qt 帳號
4. 選擇安裝路徑 (例如 `C:\Qt`)
5. 在組件選擇頁面：
   - 展開最新的 Qt 版本
   - 選擇您的編譯器 (MSVC 或 MinGW)
   - **確保勾選 Qt WebEngine**
   - 確保勾選 Qt Multimedia
6. 完成安裝

#### 設置環境變數 (Windows)

安裝後，需要將 Qt 加入 PATH：

1. 右鍵點擊 **"本機"** → **"內容"**
2. 點擊 **"進階系統設定"**
3. 點擊 **"環境變數"**
4. 在 **"系統變數"** 中找到 **Path**，點擊 **"編輯"**
5. 點擊 **"新增"**，加入 Qt 的 bin 目錄，例如：
   - `C:\Qt\6.5.0\msvc2019_64\bin`
6. 點擊 **"確定"** 儲存

## 編譯專案

### 使用 CMake (推薦)

CMake 提供更好的錯誤訊息和跨平台支援：

```bash
# 創建建置目錄
mkdir build
cd build

# 配置專案
cmake ..

# 編譯
cmake --build .

# 執行
./last-report  # Linux/macOS
last-report.exe  # Windows
```

### 使用 qmake

```bash
# Qt 6
qmake6 last-report.pro
make  # Linux/macOS
nmake  # Windows (MSVC)
mingw32-make  # Windows (MinGW)

# Qt 5
qmake last-report.pro
make
```

## 常見問題

### Q1: 錯誤 "Unknown module(s) in QT: webenginewidgets"

**原因**: Qt WebEngine 模組未安裝或未正確配置。

**解決方法**:
1. 執行 `check_webengine.sh` (Linux/macOS) 或 `check_webengine.bat` (Windows)
2. 按照腳本提示安裝缺少的模組
3. 確認安裝後重新編譯

### Q2: 找不到 qmake 命令

**Linux/macOS**:
```bash
# 檢查是否安裝
which qmake6 || which qmake

# 如果找不到，安裝 Qt
sudo apt-get install qt6-base-dev  # Ubuntu/Debian
brew install qt@6  # macOS
```

**Windows**:
- 確認 Qt 的 bin 目錄已加入 PATH
- 或使用 "Qt 6.x (MinGW/MSVC) Command Prompt"

### Q3: CMake 找不到 Qt

**解決方法**:
```bash
# 設置 Qt 路徑
export CMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64  # Linux
export CMAKE_PREFIX_PATH=/usr/local/opt/qt@6  # macOS (Homebrew)
set CMAKE_PREFIX_PATH=C:\Qt\6.x\msvc2019_64  # Windows

# 重新執行 cmake
cmake ..
```

### Q4: WebEngine 運行時錯誤

確保安裝了運行時依賴：

**Ubuntu/Debian**:
```bash
sudo apt-get install libqt6webenginecore6 libqt6webenginewidgets6
```

### Q5: Windows 上缺少 DLL

如果執行時出現缺少 DLL 錯誤：

1. 確認 Qt 的 bin 目錄在 PATH 中
2. 或將以下 DLL 複製到執行檔旁邊：
   - Qt6Core.dll
   - Qt6Gui.dll
   - Qt6Widgets.dll
   - Qt6WebEngineCore.dll
   - Qt6WebEngineWidgets.dll
   - Qt6Network.dll
   - Qt6Multimedia.dll

## 疑難排解

### 檢查 Qt 安裝

```bash
# 查詢 Qt 版本
qmake6 -query QT_VERSION

# 查詢安裝路徑
qmake6 -query QT_INSTALL_PREFIX

# 查詢函式庫路徑
qmake6 -query QT_INSTALL_LIBS

# 列出可用模組 (Linux/macOS)
ls $(qmake6 -query QT_INSTALL_LIBS) | grep WebEngine
```

### 啟用詳細輸出

```bash
# qmake 詳細輸出
qmake6 last-report.pro -d

# CMake 詳細輸出
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build . --verbose
```

### 清理建置

```bash
# qmake
make clean
rm -rf Makefile moc_* ui_* *.o

# CMake
rm -rf build/
mkdir build && cd build
```

## 獲取幫助

如果您遇到其他問題：

1. 確認已安裝所有必要的依賴
2. 檢查 Qt 版本是否符合需求 (Qt 5.15+ 或 Qt 6.x)
3. 查看完整的錯誤訊息
4. 參考 Qt 官方文件：
   - [Qt WebEngine Overview](https://doc.qt.io/qt-6/qtwebengine-overview.html)
   - [Qt for Linux/X11](https://doc.qt.io/qt-6/linux.html)
   - [Qt for macOS](https://doc.qt.io/qt-6/macos.html)
   - [Qt for Windows](https://doc.qt.io/qt-6/windows.html)

## 系統需求

### 最低需求
- **Qt**: 5.15 或更高 (推薦 Qt 6.2+)
- **編譯器**: 
  - Linux: GCC 7+ 或 Clang 5+
  - macOS: Xcode 10+ (Clang)
  - Windows: MSVC 2019+ 或 MinGW 8+
- **CMake**: 3.16 或更高 (如果使用 CMake)
- **記憶體**: 至少 4GB RAM
- **磁碟空間**: 至少 2GB

### 推薦配置
- **Qt**: 6.5 或更高
- **記憶體**: 8GB RAM 或更多
- **網路**: 穩定的寬頻連線 (用於 YouTube 串流)

---

如果本指南對您有幫助，或您有任何建議，歡迎提供回饋！
