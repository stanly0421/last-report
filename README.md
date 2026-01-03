# 音樂播放器 (Music Player)

這是一個使用 Qt6 C++ 開發的音樂播放器，靈感來自 Spotify 的設計風格。支援本地音樂檔案播放和 YouTube 影片連結。

## 快速開始

1. **編譯**: 使用 CMake 或 qmake (詳見下方說明)
2. **使用**: 貼上 YouTube 連結或選擇本地音樂檔案開始播放

## 功能特色

### 1. YouTube 影片播放
- 支援貼上 YouTube 連結直接播放
- 支援多種 YouTube URL 格式
  - https://www.youtube.com/watch?v=VIDEO_ID
  - https://youtu.be/VIDEO_ID
  - https://www.youtube.com/embed/VIDEO_ID
- 在瀏覽器中開啟影片連結

### 2. 本地音樂播放
- 支援多種音樂格式：MP3、WAV、FLAC、M4A、OGG、AAC
- 使用檔案對話框選擇音樂檔案
- 內建音樂播放器，支援播放/暫停控制
- 自動顯示歌曲名稱（從檔案名提取）

### 3. 播放清單管理
- 創建和管理多個播放清單
- 儲存 YouTube 連結和本地音樂到播放清單
- 支援從播放清單播放音樂
- 雙擊播放清單項目即可播放
- 拖放排序功能

### 4. 播放控制
- 播放/暫停按鈕（本地音樂支援實際控制）
- 上一首/下一首功能
- 隨機播放模式
- 循環播放模式
- 支援背景播放

### 5. 我的最愛功能
- 一鍵加入/移除最愛
- 自動創建「我的最愛」播放清單
- 快速訪問喜愛的音樂

### 6. Spotify 風格 UI
- 深色主題設計
- 現代化的介面佈局
- 綠色強調色 (#1DB954)
- 直覺的操作體驗
- 響應式設計

## 操作說明

### 播放 YouTube 影片
1. 複製 YouTube 影片連結
2. 在頂部搜尋欄貼上連結
3. 點擊「▶ 播放」按鈕
4. 在顯示區域點擊連結，在瀏覽器中觀看影片

### 播放本地音樂
1. 點擊頂部的「📁 本地音樂」按鈕
2. 選擇音樂檔案
3. 音樂將自動開始播放
4. 使用播放控制按鈕控制播放

### 播放控制
- **▶ 播放/⏸ 暫停**: 開始或暫停播放當前音樂（僅本地音樂支援）
- **⏮ 上一首**: 切換到上一首音樂
- **⏭ 下一首**: 切換到下一首音樂
- **🔀 隨機**: 啟用隨機播放模式
- **🔁 循環**: 啟用循環播放模式
- **❤️ 加入最愛**: 將當前音樂加入最愛

### 播放清單管理
- **➕ 新增**: 創建新的播放清單
- **🗑️ 刪除**: 刪除當前播放清單（至少保留一個）
- **雙擊音樂**: 立即播放該音樂

## 編譯和運行

### 系統需求
- Qt 6.x 或更高版本
- Qt Multimedia 模組
- C++17 編譯器
- 穩定的網路連線（播放 YouTube 影片時）

### 安裝依賴

#### Ubuntu/Debian
```bash
# 安裝 Qt 6 和必要模組
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-multimedia-dev

# 或安裝 Qt 5 (如果使用 Qt 5)
sudo apt-get install qtbase5-dev qtmultimedia5-dev
```

#### Fedora/RHEL/CentOS
```bash
# Qt 6
sudo dnf install qt6-qtbase-devel qt6-qtmultimedia-devel

# 或 Qt 5
sudo dnf install qt5-qtbase-devel qt5-qtmultimedia-devel
```

#### macOS (使用 Homebrew)
```bash
brew install qt@6
# 或 brew install qt@5
```

#### Windows
1. 下載並安裝 Qt from [qt.io](https://www.qt.io/download)
2. 在安裝時確保選擇以下組件：
   - Qt Multimedia

### 編譯

#### 使用 qmake (傳統方式)
```bash
# Qt 6
qmake6 last-report.pro
make

# Qt 5
qmake last-report.pro
make
```

#### 使用 CMake (推薦)
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 運行
```bash
./last-report
```

## 技術實現

### 核心類別
- **QMediaPlayer**: 本地音樂播放器
- **QAudioOutput**: 音訊輸出控制
- **QFileDialog**: 檔案選擇對話框
- **QListWidget**: 顯示播放清單
- **QComboBox**: 播放清單選擇器
- **QRegularExpression**: YouTube 連結解析

### YouTube 連結處理
- 使用正則表達式解析 YouTube URL
- 支援多種 URL 格式
- 生成標準觀看連結

### 本地音樂播放
- 使用 Qt Multimedia 框架
- 支援多種音訊格式
- 即時播放控制

### 資料持久化
- 使用 JSON 格式儲存播放清單
- 儲存位置：`QStandardPaths::AppDataLocation`
- 自動載入上次的播放清單
- 保存音樂資訊、檔案路徑和最愛狀態

## 界面預覽

播放器界面包含：
- **頂部搜尋欄**: Logo、YouTube 連結輸入框和本地音樂按鈕
- **左側面板**: 播放清單管理
- **中央面板**: 
  - 音樂資訊顯示
  - 連結顯示區域（YouTube 影片）
  - 播放控制按鈕

## 注意事項

### YouTube 影片播放
- YouTube 影片在瀏覽器中播放
- 需要穩定的網路連線
- 不需要 YouTube API Key

### 本地音樂播放
- 支援常見音訊格式
- 檔案路徑會被儲存在播放清單中
- 確保音樂檔案位置不會移動

## 未來改進

- 支援播放清單分享功能
- 添加音量控制滑桿
- 支援播放進度條
- 添加播放歷史記錄
- 支援深色/淺色主題切換
- 實作迷你播放器模式
- 支援更多音訊格式
- 添加等化器功能

## 疑難排解 (Troubleshooting)

### 常見問題

- **找不到 qmake**: 確認 Qt 已正確安裝並加入 PATH
- **CMake 找不到 Qt**: 設置 `CMAKE_PREFIX_PATH` 環境變數
- **缺少 DLL (Windows)**: 將 Qt bin 目錄加入 PATH 或複製所需 DLL
- **本地音樂無法播放**: 確認 Qt Multimedia 模組已正確安裝

祝您使用愉快！🎵