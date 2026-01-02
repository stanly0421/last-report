# YouTube 音樂播放器 (YouTube Music Player)

這是一個使用 Qt6 C++ 和 Qt WebEngine 開發的 YouTube 音樂播放器，靈感來自 Spotify 的設計風格。

## 功能特色

### 1. YouTube 影片搜尋
- 使用 YouTube Data API v3 搜尋影片
- 顯示搜尋結果包含影片標題、頻道名稱
- 支援關鍵字搜尋音樂和影片
- 即時顯示最多 20 筆搜尋結果

### 2. 嵌入式影片播放
- 使用 Qt WebEngineView 嵌入 YouTube 播放器
- 合法使用 YouTube embed API
- 自動播放功能
- 流暢的播放體驗

### 3. 播放清單管理
- 創建和管理多個播放清單
- 從搜尋結果添加影片到播放清單
- 支援刪除播放清單中的影片
- 雙擊播放清單項目即可播放
- 拖放排序功能

### 4. 播放控制
- 播放/暫停按鈕
- 上一首/下一首功能
- 隨機播放模式
- 循環播放模式
- 支援背景播放

### 5. 我的最愛功能
- 一鍵加入/移除最愛
- 自動創建「我的最愛」播放清單
- 快速訪問喜愛的影片

### 6. Spotify 風格 UI
- 深色主題設計
- 現代化的介面佈局
- 綠色強調色 (#1DB954)
- 直覺的操作體驗
- 響應式設計

## 操作說明

### 搜尋影片
1. 在頂部搜尋欄輸入關鍵字
2. 點擊「🔍 搜尋」按鈕或按 Enter
3. 在搜尋結果中雙擊影片即可播放
4. 選擇影片後點擊「➕ 加入」將其添加到當前播放清單

### 播放控制
- **▶ 播放/⏸ 暫停**: 開始或暫停播放當前影片
- **⏮ 上一首**: 切換到上一首影片
- **⏭ 下一首**: 切換到下一首影片
- **🔀 隨機**: 啟用隨機播放模式
- **🔁 循環**: 啟用循環播放模式
- **❤️ 加入最愛**: 將當前影片加入最愛

### 播放清單管理
- **➕ 新增**: 創建新的播放清單
- **🗑️ 刪除**: 刪除當前播放清單（至少保留一個）
- **➕ 加入**: 將選中的搜尋結果加入播放清單
- **➖ 移除**: 從播放清單移除選中的影片
- **雙擊影片**: 立即播放該影片

## 編譯和運行

### 系統需求
- Qt 6.x 或更高版本
- **Qt WebEngine 模組** (必須安裝)
- Qt Network 模組
- Qt Multimedia 模組
- C++17 編譯器
- 穩定的網路連線

### 安裝依賴

#### Ubuntu/Debian
```bash
# 安裝 Qt 6 和必要模組
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-webengine-dev qt6-multimedia-dev

# 或安裝 Qt 5 (如果使用 Qt 5)
sudo apt-get install qtbase5-dev qtwebengine5-dev qtmultimedia5-dev
```

#### Fedora/RHEL/CentOS
```bash
# Qt 6
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel qt6-qtmultimedia-devel

# 或 Qt 5
sudo dnf install qt5-qtbase-devel qt5-qtwebengine-devel qt5-qtmultimedia-devel
```

#### macOS (使用 Homebrew)
```bash
brew install qt@6
# 或 brew install qt@5
```

#### Windows
1. 下載並安裝 Qt from [qt.io](https://www.qt.io/download)
2. 在安裝時確保選擇以下組件：
   - Qt WebEngine
   - Qt Multimedia
   - Qt Network

### 驗證 Qt WebEngine 安裝

安裝完成後，驗證 WebEngine 模組是否可用：

```bash
# Qt 6
qmake6 -query | grep WebEngine

# Qt 5
qmake -query QT_INSTALL_LIBS
ls $(qmake -query QT_INSTALL_LIBS) | grep WebEngine
```

如果找不到 WebEngine 相關檔案，請重新安裝 Qt WebEngine 模組。

### 設置 YouTube API Key
在使用前，您需要獲取 YouTube Data API v3 的 API Key：

1. 前往 [Google Cloud Console](https://console.cloud.google.com/)
2. 創建新專案或選擇現有專案
3. 啟用 YouTube Data API v3
4. 創建憑證（API Key）
5. 在 `widget.cpp` 中替換 `apiKey` 變數的值

```cpp
apiKey("YOUR_API_KEY_HERE")
```

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

**注意**: 如果遇到 "Unknown module(s) in QT: webenginewidgets" 錯誤，表示 Qt WebEngine 未正確安裝。請參考上面的安裝指令重新安裝。

### 運行
```bash
./last-report
```

## 技術實現

### 核心類別
- **QWebEngineView**: YouTube 嵌入播放器
- **QNetworkAccessManager**: YouTube API 網路請求
- **QJsonDocument**: 解析 API 回應
- **QListWidget**: 顯示搜尋結果和播放清單
- **QComboBox**: 播放清單選擇器

### YouTube API 整合
- 使用 YouTube Data API v3 的 search endpoint
- 搜尋參數：`type=video`, `maxResults=20`
- 回傳影片資訊：videoId, title, channelTitle, thumbnails
- 使用 embed URL 格式：`https://www.youtube.com/embed/{videoId}`

### 資料持久化
- 使用 JSON 格式儲存播放清單
- 儲存位置：`QStandardPaths::AppDataLocation`
- 自動載入上次的播放清單
- 保存影片資訊和最愛狀態

## 界面預覽

播放器界面包含：
- **頂部搜尋欄**: Logo 和搜尋功能
- **左側面板**: 播放清單管理
- **中央面板**: 
  - 影片資訊顯示
  - YouTube 嵌入播放器
  - 播放控制按鈕
  - 搜尋結果列表

## 注意事項

### API 配額限制
- YouTube Data API 有每日配額限制
- 預設配額：10,000 單位/天
- search 操作消耗：100 單位/次
- 建議合理使用，避免頻繁搜尋

### 網路連線
- 需要穩定的網路連線
- 影片播放需要足夠的頻寬
- 建議使用 Wi-Fi 或高速網路

### 背景播放
- 當應用程式在背景執行時，影片仍會播放
- 可以最小化視窗繼續聆聽音樂
- 系統音量控制有效

## 未來改進

- 支援播放清單分享功能
- 添加影片品質選擇
- 支援歌詞顯示（如果有）
- 實作真正的背景播放（純音訊）
- 添加播放歷史記錄
- 支援深色/淺色主題切換
- 實作迷你播放器模式

## 授權聲明

本專案使用 YouTube API 服務，必須遵守 [YouTube API 服務條款](https://developers.google.com/youtube/terms/api-services-terms-of-service)。

祝您使用愉快！🎵