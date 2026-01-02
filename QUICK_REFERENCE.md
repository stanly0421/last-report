# 快速參考 (Quick Reference)

本文件提供快速命令參考，幫助您快速上手編譯和運行專案。

## 📋 前置檢查

```bash
# Linux/macOS: 檢查 Qt WebEngine 是否已安裝
./check_webengine.sh

# Windows: 檢查 Qt WebEngine 是否已安裝
check_webengine.bat
```

## 🔧 安裝依賴

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-webengine-dev qt6-multimedia-dev
```

### Fedora/RHEL
```bash
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel qt6-qtmultimedia-devel
```

### macOS
```bash
brew install qt@6
```

### Windows
使用 Qt Maintenance Tool 或從 https://www.qt.io/download 下載安裝

## 🏗️ 編譯專案

### 方法 1: CMake (推薦)

```bash
# 創建並進入建置目錄
mkdir build && cd build

# 配置專案
cmake ..

# 編譯
cmake --build .

# 執行
./last-report          # Linux/macOS
last-report.exe        # Windows
```

### 方法 2: qmake

```bash
# Qt 6
qmake6 last-report.pro
make                   # Linux/macOS
nmake                  # Windows (MSVC)
mingw32-make           # Windows (MinGW)

# Qt 5
qmake last-report.pro
make
```

## 🧹 清理建置

### CMake
```bash
# 刪除建置目錄
rm -rf build

# 或在建置目錄內
cd build
cmake --build . --target clean
```

### qmake
```bash
make clean
make distclean         # 更徹底的清理
```

## 🐛 疑難排解

### 錯誤: "Unknown module(s) in QT: webenginewidgets"
```bash
# 1. 執行檢查腳本
./check_webengine.sh

# 2. 安裝缺少的模組 (Ubuntu 範例)
sudo apt-get install qt6-webengine-dev

# 3. 重新編譯
```

### 錯誤: 找不到 qmake
```bash
# 檢查 qmake 位置
which qmake6 || which qmake

# 如果找不到，將 Qt bin 目錄加入 PATH
export PATH=/path/to/Qt/6.x/gcc_64/bin:$PATH  # Linux/macOS
set PATH=C:\Qt\6.x\msvc2019_64\bin;%PATH%     # Windows
```

### 錯誤: CMake 找不到 Qt
```bash
# 設置 Qt 路徑
export CMAKE_PREFIX_PATH=/path/to/Qt/6.x/gcc_64     # Linux
export CMAKE_PREFIX_PATH=/usr/local/opt/qt@6        # macOS (Homebrew)
set CMAKE_PREFIX_PATH=C:\Qt\6.x\msvc2019_64         # Windows

# 重新執行 cmake
cmake ..
```

## 🔍 查詢 Qt 資訊

```bash
# 查詢 Qt 版本
qmake6 -query QT_VERSION

# 查詢安裝路徑
qmake6 -query QT_INSTALL_PREFIX

# 查詢函式庫路徑
qmake6 -query QT_INSTALL_LIBS

# 列出 WebEngine 相關檔案 (Linux/macOS)
ls $(qmake6 -query QT_INSTALL_LIBS) | grep WebEngine
```

## 📝 設置 API Key

1. 前往 [Google Cloud Console](https://console.cloud.google.com/)
2. 創建專案並啟用 YouTube Data API v3
3. 創建 API Key
4. 編輯 `widget.cpp`，找到:
   ```cpp
   apiKey("YOUR_API_KEY_HERE")
   ```
5. 替換為您的 API Key
6. 重新編譯專案

## 🚀 開發工作流程

### 標準流程
```bash
# 1. 檢查依賴
./check_webengine.sh

# 2. 編譯 (首次)
mkdir build && cd build
cmake ..
cmake --build .

# 3. 執行測試
./last-report

# 4. 修改代碼後重新編譯
cmake --build .

# 5. 再次測試
./last-report
```

### 快速重新編譯
```bash
# 在 build 目錄內
cd build
cmake --build . && ./last-report
```

## 🧪 除錯建置

### 詳細輸出

```bash
# qmake 詳細輸出
qmake6 last-report.pro -d

# CMake 詳細輸出
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build . --verbose

# Make 詳細輸出
make VERBOSE=1
```

### 檢查編譯器

```bash
# 檢查 C++ 編譯器
c++ --version       # Linux/macOS
cl                  # Windows (MSVC)

# 檢查 CMake
cmake --version

# 檢查 Make
make --version      # Linux/macOS
nmake /?            # Windows (MSVC)
```

## 📚 更多資訊

- **完整安裝指南**: [INSTALL.md](INSTALL.md)
- **使用者手冊**: [USER_GUIDE.md](USER_GUIDE.md)
- **測試指南**: [TESTING.md](TESTING.md)
- **專案狀態**: [PROJECT_STATUS.txt](PROJECT_STATUS.txt)

## 💡 提示

- 使用 CMake 可以獲得更好的錯誤訊息
- 首次編譯可能需要較長時間
- 確保有穩定的網路連線（用於 YouTube API）
- 定期清理建置目錄避免問題
- 遇到問題先執行 `check_webengine.sh`

---

如果遇到本文未涵蓋的問題，請參考 [INSTALL.md](INSTALL.md) 的詳細疑難排解章節。
