#include "widget.h"
#include "ui_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFile>
#include <QDir>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QSplitter>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , networkManager(new QNetworkAccessManager(this))
    , apiKey("YOUR_YOUTUBE_API_KEY_HERE")  // 請使用 setup_api_key.sh 或 setup_api_key.bat 設置您的 API Key
    , currentPlaylistIndex(-1)
    , currentVideoIndex(-1)
    , isShuffleMode(false)
    , isRepeatMode(false)
    , isPlaying(false)
{
    ui->setupUi(this);
    
    // 設置窗口
    setWindowTitle("YouTube 音樂播放器");
    setMinimumSize(1000, 700);
    
    // 建立UI
    setupUI();
    
    // 建立信號連接
    createConnections();
    
    // 加載保存的播放清單
    loadPlaylistsFromFile();
    
    // 如果沒有播放清單，創建默認播放清單
    if (playlists.isEmpty()) {
        Playlist defaultPlaylist;
        defaultPlaylist.name = "我的播放清單";
        playlists.append(defaultPlaylist);
        
        Playlist favoritesPlaylist;
        favoritesPlaylist.name = "我的最愛";
        playlists.append(favoritesPlaylist);
        
        playlistComboBox->addItem(defaultPlaylist.name);
        playlistComboBox->addItem(favoritesPlaylist.name);
        currentPlaylistIndex = 0;
    } else {
        // 恢復播放清單到ComboBox
        for (const Playlist& playlist : playlists) {
            playlistComboBox->addItem(playlist.name);
        }
        
        // 恢復上次的播放清單
        int lastIndex = 0;
        for (int i = 0; i < playlists.size(); i++) {
            if (playlists[i].name == lastPlaylistName) {
                lastIndex = i;
                break;
            }
        }
        playlistComboBox->setCurrentIndex(lastIndex);
        currentPlaylistIndex = lastIndex;
        updatePlaylistDisplay();
    }
    
    // 更新按鈕狀態
    updateButtonStates();
}

Widget::~Widget()
{
    // 保存播放清單
    savePlaylistsToFile();
    delete ui;
}

void Widget::setupUI()
{
    // 主佈局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 設置深色主題
    setStyleSheet(
        "QWidget {"
        "   background-color: #121212;"
        "   color: #FFFFFF;"
        "}"
        "QLineEdit {"
        "   background-color: #282828;"
        "   border: 1px solid #404040;"
        "   border-radius: 20px;"
        "   padding: 8px 16px;"
        "   color: #FFFFFF;"
        "   font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid #1DB954;"
        "}"
        "QListWidget {"
        "   background-color: #181818;"
        "   border: none;"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   padding: 10px;"
        "   border-bottom: 1px solid #282828;"
        "   color: #B3B3B3;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #282828;"
        "   color: #FFFFFF;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #1DB954;"
        "   color: #FFFFFF;"
        "}"
        "QComboBox {"
        "   background-color: #282828;"
        "   border: 1px solid #404040;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "   color: #FFFFFF;"
        "   min-width: 150px;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #282828;"
        "   color: #FFFFFF;"
        "   selection-background-color: #1DB954;"
        "}"
    );
    
    // === 頂部搜尋欄 ===
    QWidget* topBar = new QWidget(this);
    topBar->setStyleSheet("background-color: #000000; padding: 16px;");
    QHBoxLayout* topLayout = new QHBoxLayout(topBar);
    
    QLabel* logoLabel = new QLabel("🎵 YouTube Player", topBar);
    logoLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #1DB954;");
    topLayout->addWidget(logoLabel);
    
    topLayout->addStretch();
    
    searchEdit = new QLineEdit(topBar);
    searchEdit->setPlaceholderText("搜尋歌曲或影片...");
    searchEdit->setMinimumWidth(400);
    topLayout->addWidget(searchEdit);
    
    searchButton = new QPushButton("🔍 搜尋", topBar);
    searchButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #1DB954;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 8px 24px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #1ED760; }"
        "QPushButton:pressed { background-color: #1AA34A; }"
    );
    topLayout->addWidget(searchButton);
    
    mainLayout->addWidget(topBar);
    
    // === 內容區域 ===
    QSplitter* contentSplitter = new QSplitter(Qt::Horizontal, this);
    contentSplitter->setStyleSheet("QSplitter::handle { background-color: #282828; }");
    
    // === 左側面板：播放清單 ===
    QWidget* leftPanel = new QWidget(contentSplitter);
    leftPanel->setStyleSheet("background-color: #000000;");
    leftPanel->setMinimumWidth(250);
    leftPanel->setMaximumWidth(350);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(16, 16, 16, 16);
    leftLayout->setSpacing(12);
    
    QLabel* playlistLabel = new QLabel("播放清單", leftPanel);
    playlistLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF; margin-bottom: 8px;");
    leftLayout->addWidget(playlistLabel);
    
    playlistComboBox = new QComboBox(leftPanel);
    leftLayout->addWidget(playlistComboBox);
    
    QHBoxLayout* playlistButtonLayout = new QHBoxLayout();
    
    newPlaylistButton = new QPushButton("➕ 新增", leftPanel);
    newPlaylistButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
    );
    playlistButtonLayout->addWidget(newPlaylistButton);
    
    deletePlaylistButton = new QPushButton("🗑️ 刪除", leftPanel);
    deletePlaylistButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
    );
    playlistButtonLayout->addWidget(deletePlaylistButton);
    
    leftLayout->addLayout(playlistButtonLayout);
    
    playlistWidget = new QListWidget(leftPanel);
    leftLayout->addWidget(playlistWidget);
    
    QHBoxLayout* videoButtonLayout = new QHBoxLayout();
    
    addToPlaylistButton = new QPushButton("➕ 加入", leftPanel);
    addToPlaylistButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
        "QPushButton:disabled { background-color: #181818; color: #404040; }"
    );
    videoButtonLayout->addWidget(addToPlaylistButton);
    
    removeVideoButton = new QPushButton("➖ 移除", leftPanel);
    removeVideoButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
        "QPushButton:disabled { background-color: #181818; color: #404040; }"
    );
    videoButtonLayout->addWidget(removeVideoButton);
    
    leftLayout->addLayout(videoButtonLayout);
    
    contentSplitter->addWidget(leftPanel);
    
    // === 中央面板：影片播放器和搜尋結果 ===
    QWidget* centerPanel = new QWidget(contentSplitter);
    centerPanel->setStyleSheet("background-color: #121212;");
    QVBoxLayout* centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(16, 16, 16, 16);
    centerLayout->setSpacing(16);
    
    // 影片資訊
    videoTitleLabel = new QLabel("選擇一首歌曲開始播放", centerPanel);
    videoTitleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFFFFF;");
    videoTitleLabel->setWordWrap(true);
    centerLayout->addWidget(videoTitleLabel);
    
    channelLabel = new QLabel("", centerPanel);
    channelLabel->setStyleSheet("font-size: 14px; color: #B3B3B3;");
    centerLayout->addWidget(channelLabel);
    
    // 影片播放器
    webEngineView = new QWebEngineView(centerPanel);
    webEngineView->setMinimumHeight(400);
    webEngineView->setStyleSheet("background-color: #000000;");
    centerLayout->addWidget(webEngineView, 1);
    
    // 播放控制區域
    QWidget* controlWidget = new QWidget(centerPanel);
    controlWidget->setStyleSheet("background-color: #181818; border-radius: 8px; padding: 16px;");
    QHBoxLayout* controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setSpacing(12);
    
    QString buttonStyle = 
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #FFFFFF;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   min-width: 40px;"
        "}"
        "QPushButton:hover { background-color: #404040; }"
        "QPushButton:pressed { background-color: #505050; }"
        "QPushButton:disabled { background-color: #181818; color: #404040; }";
    
    shuffleButton = new QPushButton("🔀", controlWidget);
    shuffleButton->setStyleSheet(buttonStyle);
    shuffleButton->setCheckable(true);
    shuffleButton->setToolTip("隨機播放");
    controlLayout->addWidget(shuffleButton);
    
    previousButton = new QPushButton("⏮", controlWidget);
    previousButton->setStyleSheet(buttonStyle);
    previousButton->setToolTip("上一首");
    controlLayout->addWidget(previousButton);
    
    playPauseButton = new QPushButton("▶", controlWidget);
    playPauseButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #1DB954;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 25px;"
        "   padding: 12px;"
        "   font-size: 18px;"
        "   min-width: 50px;"
        "   min-height: 50px;"
        "}"
        "QPushButton:hover { background-color: #1ED760; }"
        "QPushButton:pressed { background-color: #1AA34A; }"
        "QPushButton:disabled { background-color: #282828; color: #404040; }"
    );
    controlLayout->addWidget(playPauseButton);
    
    nextButton = new QPushButton("⏭", controlWidget);
    nextButton->setStyleSheet(buttonStyle);
    nextButton->setToolTip("下一首");
    controlLayout->addWidget(nextButton);
    
    repeatButton = new QPushButton("🔁", controlWidget);
    repeatButton->setStyleSheet(buttonStyle);
    repeatButton->setCheckable(true);
    repeatButton->setToolTip("循環播放");
    controlLayout->addWidget(repeatButton);
    
    controlLayout->addStretch();
    
    toggleFavoriteButton = new QPushButton("❤️ 加入最愛", controlWidget);
    toggleFavoriteButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #282828;"
        "   color: #B3B3B3;"
        "   border: none;"
        "   border-radius: 20px;"
        "   padding: 10px 20px;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover { background-color: #404040; color: #FFFFFF; }"
        "QPushButton:disabled { background-color: #181818; color: #404040; }"
    );
    toggleFavoriteButton->setEnabled(false);
    controlLayout->addWidget(toggleFavoriteButton);
    
    centerLayout->addWidget(controlWidget);
    
    // 搜尋結果
    QLabel* searchResultLabel = new QLabel("搜尋結果", centerPanel);
    searchResultLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #FFFFFF; margin-top: 16px;");
    centerLayout->addWidget(searchResultLabel);
    
    searchResultsWidget = new QListWidget(centerPanel);
    searchResultsWidget->setMinimumHeight(200);
    centerLayout->addWidget(searchResultsWidget);
    
    contentSplitter->addWidget(centerPanel);
    
    // 設置分割器比例
    contentSplitter->setStretchFactor(0, 1);
    contentSplitter->setStretchFactor(1, 3);
    
    mainLayout->addWidget(contentSplitter, 1);
}

void Widget::createConnections()
{
    // 搜尋功能
    connect(searchButton, &QPushButton::clicked, this, &Widget::onSearchClicked);
    connect(searchEdit, &QLineEdit::returnPressed, this, &Widget::onSearchClicked);
    
    // 播放控制按鈕
    connect(playPauseButton, &QPushButton::clicked, this, &Widget::onPlayPauseClicked);
    connect(previousButton, &QPushButton::clicked, this, &Widget::onPreviousClicked);
    connect(nextButton, &QPushButton::clicked, this, &Widget::onNextClicked);
    connect(shuffleButton, &QPushButton::clicked, this, &Widget::onShuffleClicked);
    connect(repeatButton, &QPushButton::clicked, this, &Widget::onRepeatClicked);
    
    // 播放清單管理
    connect(addToPlaylistButton, &QPushButton::clicked, this, &Widget::onAddToPlaylistClicked);
    connect(removeVideoButton, &QPushButton::clicked, this, &Widget::onRemoveVideoClicked);
    connect(playlistWidget, &QListWidget::itemDoubleClicked, this, &Widget::onVideoDoubleClicked);
    connect(playlistWidget, &QListWidget::itemSelectionChanged, this, &Widget::updateButtonStates);
    
    // 搜尋結果
    connect(searchResultsWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        int index = searchResultsWidget->row(item);
        if (index >= 0 && index < searchResults.size()) {
            // 直接播放搜尋結果中的影片
            VideoInfo video = searchResults[index];
            currentVideoIndex = -1;  // 不屬於播放清單
            
            QString embedUrl = QString("https://www.youtube.com/embed/%1?autoplay=1").arg(video.videoId);
            webEngineView->setUrl(QUrl(embedUrl));
            
            videoTitleLabel->setText(video.title);
            channelLabel->setText(video.channelTitle);
            isPlaying = true;
            playPauseButton->setText("⏸");
            updateButtonStates();
        }
    });
    
    // 最愛按鈕
    connect(toggleFavoriteButton, &QPushButton::clicked, this, &Widget::onToggleFavoriteClicked);
    
    // 播放清單選擇
    connect(newPlaylistButton, &QPushButton::clicked, this, &Widget::onNewPlaylistClicked);
    connect(deletePlaylistButton, &QPushButton::clicked, this, &Widget::onDeletePlaylistClicked);
    connect(playlistComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Widget::onPlaylistChanged);
    
    // 網路請求
    connect(networkManager, &QNetworkAccessManager::finished, this, &Widget::onNetworkReplyFinished);
}

void Widget::onSearchClicked()
{
    QString query = searchEdit->text().trimmed();
    if (query.isEmpty()) {
        QMessageBox::warning(this, "搜尋", "請輸入搜尋關鍵字！");
        return;
    }
    
    searchYouTube(query);
}

void Widget::searchYouTube(const QString& query)
{
    QString url = QString("https://www.googleapis.com/youtube/v3/search"
                         "?part=snippet"
                         "&q=%1"
                         "&type=video"
                         "&maxResults=20"
                         "&key=%2")
                         .arg(QString(QUrl::toPercentEncoding(query)))
                         .arg(apiKey);
    
    QNetworkRequest request(url);
    networkManager->get(request);
}

void Widget::onNetworkReplyFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("items")) {
                QJsonArray items = obj["items"].toArray();
                showSearchResults(items);
            }
        }
    } else {
        QMessageBox::warning(this, "錯誤", 
            QString("搜尋失敗：%1\n\n請確認：\n1. 網路連線正常\n2. API Key 有效\n3. YouTube API 配額未超過")
            .arg(reply->errorString()));
    }
    
    reply->deleteLater();
}

void Widget::showSearchResults(const QJsonArray& items)
{
    searchResults.clear();
    searchResultsWidget->clear();
    
    for (const QJsonValue& value : items) {
        QJsonObject item = value.toObject();
        QJsonObject id = item["id"].toObject();
        QJsonObject snippet = item["snippet"].toObject();
        
        VideoInfo video;
        video.videoId = id["videoId"].toString();
        video.title = snippet["title"].toString();
        video.channelTitle = snippet["channelTitle"].toString();
        video.description = snippet["description"].toString();
        
        QJsonObject thumbnails = snippet["thumbnails"].toObject();
        if (thumbnails.contains("default")) {
            video.thumbnailUrl = thumbnails["default"].toObject()["url"].toString();
        }
        
        video.isFavorite = false;
        searchResults.append(video);
        
        QString displayText = QString("%1\n%2").arg(video.title).arg(video.channelTitle);
        QListWidgetItem* item = new QListWidgetItem(displayText);
        searchResultsWidget->addItem(item);
    }
    
    if (searchResults.isEmpty()) {
        QListWidgetItem* item = new QListWidgetItem("沒有找到結果");
        item->setFlags(Qt::NoItemFlags);
        searchResultsWidget->addItem(item);
    }
}

void Widget::onPlayPauseClicked()
{
    if (currentVideoIndex >= 0) {
        // 有正在播放的影片
        isPlaying = !isPlaying;
        playPauseButton->setText(isPlaying ? "⏸" : "▶");
        
        // 注意：完整的播放/暫停控制需要通過 JavaScript 與 YouTube iframe API 互動
        // 目前實作為簡化版本，實際播放控制由 YouTube 嵌入播放器處理
        // 未來可以通過 QWebEngineView::page()->runJavaScript() 實現完整控制
    } else {
        // 沒有影片，播放播放清單第一首
        if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
            Playlist& playlist = playlists[currentPlaylistIndex];
            if (!playlist.videos.isEmpty()) {
                playVideo(0);
            }
        }
    }
}

void Widget::onPreviousClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.videos.isEmpty()) return;
    
    if (isShuffleMode) {
        int newIndex = getRandomVideoIndex(true);
        if (newIndex >= 0) {
            playVideo(newIndex);
        }
    } else {
        int newIndex = currentVideoIndex - 1;
        if (newIndex < 0) {
            newIndex = playlist.videos.size() - 1;
        }
        playVideo(newIndex);
    }
}

void Widget::onNextClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.videos.isEmpty()) return;
    
    int newIndex = getNextVideoIndex();
    if (newIndex >= 0) {
        playVideo(newIndex);
    }
}

void Widget::onShuffleClicked()
{
    isShuffleMode = !isShuffleMode;
    shuffleButton->setChecked(isShuffleMode);
    
    if (isShuffleMode) {
        playedVideosInCurrentSession.clear();
        shuffleButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #1DB954;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #1ED760; }"
        );
    } else {
        shuffleButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #282828;"
            "   color: #FFFFFF;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #404040; }"
        );
    }
}

void Widget::onRepeatClicked()
{
    isRepeatMode = !isRepeatMode;
    repeatButton->setChecked(isRepeatMode);
    
    if (isRepeatMode) {
        repeatButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #1DB954;"
            "   color: white;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #1ED760; }"
        );
    } else {
        repeatButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #282828;"
            "   color: #FFFFFF;"
            "   border: none;"
            "   border-radius: 20px;"
            "   padding: 10px 20px;"
            "   font-size: 14px;"
            "   min-width: 40px;"
            "}"
            "QPushButton:hover { background-color: #404040; }"
        );
    }
}

void Widget::onAddToPlaylistClicked()
{
    int selectedRow = searchResultsWidget->currentRow();
    if (selectedRow < 0 || selectedRow >= searchResults.size()) {
        QMessageBox::information(this, "加入播放清單", "請先選擇一個搜尋結果！");
        return;
    }
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    VideoInfo video = searchResults[selectedRow];
    Playlist& playlist = playlists[currentPlaylistIndex];
    
    // 檢查是否已存在
    bool exists = false;
    for (const VideoInfo& v : playlist.videos) {
        if (v.videoId == video.videoId) {
            exists = true;
            break;
        }
    }
    
    if (exists) {
        QMessageBox::information(this, "加入播放清單", "此影片已在播放清單中！");
        return;
    }
    
    playlist.videos.append(video);
    updatePlaylistDisplay();
    updateButtonStates();
    
    QMessageBox::information(this, "加入播放清單", 
        QString("已將「%1」加入到「%2」！").arg(video.title).arg(playlist.name));
}

void Widget::onRemoveVideoClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    int selectedRow = playlistWidget->currentRow();
    if (selectedRow < 0) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (selectedRow < playlist.videos.size()) {
        if (selectedRow == currentVideoIndex) {
            webEngineView->setUrl(QUrl("about:blank"));
            currentVideoIndex = -1;
            isPlaying = false;
        } else if (selectedRow < currentVideoIndex) {
            currentVideoIndex--;
        }
        
        playlist.videos.removeAt(selectedRow);
        updatePlaylistDisplay();
        updateButtonStates();
    }
}

void Widget::onVideoDoubleClicked(QListWidgetItem* item)
{
    int index = playlistWidget->row(item);
    playVideo(index);
}

void Widget::onToggleFavoriteClicked()
{
    if (currentVideoIndex < 0 || currentPlaylistIndex < 0) return;
    if (currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& currentPlaylist = playlists[currentPlaylistIndex];
    if (currentVideoIndex >= currentPlaylist.videos.size()) return;
    
    VideoInfo& video = currentPlaylist.videos[currentVideoIndex];
    
    // 找到 "我的最愛" 播放清單
    int favoritesIndex = -1;
    for (int i = 0; i < playlists.size(); i++) {
        if (playlists[i].name == "我的最愛") {
            favoritesIndex = i;
            break;
        }
    }
    
    if (favoritesIndex < 0) {
        // 創建 "我的最愛" 播放清單
        Playlist favoritesPlaylist;
        favoritesPlaylist.name = "我的最愛";
        playlists.append(favoritesPlaylist);
        playlistComboBox->addItem(favoritesPlaylist.name);
        favoritesIndex = playlists.size() - 1;
    }
    
    Playlist& favoritesPlaylist = playlists[favoritesIndex];
    
    // 檢查是否已在最愛中
    bool isInFavorites = false;
    int favoriteIndex = -1;
    for (int i = 0; i < favoritesPlaylist.videos.size(); i++) {
        if (favoritesPlaylist.videos[i].videoId == video.videoId) {
            isInFavorites = true;
            favoriteIndex = i;
            break;
        }
    }
    
    if (isInFavorites) {
        // 從最愛移除
        favoritesPlaylist.videos.removeAt(favoriteIndex);
        video.isFavorite = false;
        toggleFavoriteButton->setText("❤️ 加入最愛");
        QMessageBox::information(this, "我的最愛", "已從最愛中移除！");
    } else {
        // 加入最愛
        VideoInfo favoriteVideo = video;
        favoriteVideo.isFavorite = true;
        favoritesPlaylist.videos.append(favoriteVideo);
        video.isFavorite = true;
        toggleFavoriteButton->setText("💔 移除最愛");
        QMessageBox::information(this, "我的最愛", "已加入最愛！");
    }
    
    updatePlaylistDisplay();
}

void Widget::onNewPlaylistClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增播放清單", 
                                         "請輸入播放清單名稱:", 
                                         QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        // 檢查是否重複
        for (const Playlist& p : playlists) {
            if (p.name == name) {
                QMessageBox::warning(this, "新增播放清單", "播放清單名稱已存在！");
                return;
            }
        }
        
        Playlist newPlaylist;
        newPlaylist.name = name;
        playlists.append(newPlaylist);
        playlistComboBox->addItem(name);
        
        int newIndex = playlists.size() - 1;
        playlistComboBox->setCurrentIndex(newIndex);
        currentPlaylistIndex = newIndex;
        lastPlaylistName = name;
        updatePlaylistDisplay();
        updateButtonStates();
    }
}

void Widget::onDeletePlaylistClicked()
{
    if (playlists.size() <= 1) {
        QMessageBox::warning(this, "無法刪除", "至少需要保留一個播放清單！");
        return;
    }
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    int ret = QMessageBox::question(this, "確認刪除", 
                                    QString("確定要刪除播放清單「%1」嗎？")
                                    .arg(playlists[currentPlaylistIndex].name),
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        webEngineView->setUrl(QUrl("about:blank"));
        currentVideoIndex = -1;
        isPlaying = false;
        playlists.removeAt(currentPlaylistIndex);
        playlistComboBox->removeItem(currentPlaylistIndex);
    }
}

void Widget::onPlaylistChanged(int index)
{
    if (index < 0 || index >= playlists.size()) return;
    
    currentPlaylistIndex = index;
    currentVideoIndex = -1;
    playedVideosInCurrentSession.clear();
    updatePlaylistDisplay();
    updateButtonStates();
}

void Widget::updatePlaylistDisplay()
{
    playlistWidget->clear();
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    const Playlist& playlist = playlists[currentPlaylistIndex];
    for (int i = 0; i < playlist.videos.size(); i++) {
        const VideoInfo& video = playlist.videos[i];
        QString displayText = QString("%1. %2\n   %3")
                                .arg(i + 1)
                                .arg(video.title)
                                .arg(video.channelTitle);
        
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, i);
        
        // 高亮當前播放的影片
        if (i == currentVideoIndex) {
            item->setBackground(QColor("#1DB954"));
            item->setForeground(QColor("#FFFFFF"));
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
        }
        
        playlistWidget->addItem(item);
    }
}

void Widget::playVideo(int index)
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (index < 0 || index >= playlist.videos.size()) return;
    
    currentVideoIndex = index;
    const VideoInfo& video = playlist.videos[index];
    
    playedVideosInCurrentSession.insert(index);
    
    // 載入 YouTube 嵌入播放器
    QString embedUrl = QString("https://www.youtube.com/embed/%1?autoplay=1").arg(video.videoId);
    webEngineView->setUrl(QUrl(embedUrl));
    
    // 更新顯示
    videoTitleLabel->setText(video.title);
    channelLabel->setText(video.channelTitle);
    isPlaying = true;
    playPauseButton->setText("⏸");
    
    // 更新最愛按鈕
    if (video.isFavorite) {
        toggleFavoriteButton->setText("💔 移除最愛");
    } else {
        toggleFavoriteButton->setText("❤️ 加入最愛");
    }
    
    updatePlaylistDisplay();
    updateButtonStates();
    
    playlistWidget->setCurrentRow(index);
}

void Widget::updateButtonStates()
{
    bool hasPlaylist = (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size());
    bool hasVideos = hasPlaylist && !playlists[currentPlaylistIndex].videos.isEmpty();
    int selectedRow = playlistWidget->currentRow();
    bool hasSelection = selectedRow >= 0;
    bool hasVideoPlaying = currentVideoIndex >= 0;
    bool hasSearchResult = searchResultsWidget->currentRow() >= 0;
    
    playPauseButton->setEnabled(hasVideos);
    previousButton->setEnabled(hasVideos);
    nextButton->setEnabled(hasVideos);
    removeVideoButton->setEnabled(hasSelection);
    deletePlaylistButton->setEnabled(playlists.size() > 1);
    toggleFavoriteButton->setEnabled(hasVideoPlaying);
    addToPlaylistButton->setEnabled(hasSearchResult);
}

void Widget::savePlaylistsToFile()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(configDir)) {
        dir.mkpath(configDir);
    }
    
    QString configFile = configDir + "/youtube_playlists.json";
    
    QJsonObject rootObj;
    QJsonArray playlistsArray;
    
    for (const Playlist& playlist : playlists) {
        QJsonObject playlistObj;
        playlistObj["name"] = playlist.name;
        
        QJsonArray videosArray;
        for (const VideoInfo& video : playlist.videos) {
            QJsonObject videoObj;
            videoObj["videoId"] = video.videoId;
            videoObj["title"] = video.title;
            videoObj["channelTitle"] = video.channelTitle;
            videoObj["thumbnailUrl"] = video.thumbnailUrl;
            videoObj["description"] = video.description;
            videoObj["isFavorite"] = video.isFavorite;
            videosArray.append(videoObj);
        }
        playlistObj["videos"] = videosArray;
        playlistsArray.append(playlistObj);
    }
    
    rootObj["playlists"] = playlistsArray;
    if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
        rootObj["lastPlaylist"] = playlists[currentPlaylistIndex].name;
    }
    
    QJsonDocument doc(rootObj);
    QFile file(configFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void Widget::loadPlaylistsFromFile()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString configFile = configDir + "/youtube_playlists.json";
    
    QFile file(configFile);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return;
    }
    
    QJsonObject rootObj = doc.object();
    lastPlaylistName = rootObj["lastPlaylist"].toString();
    
    QJsonArray playlistsArray = rootObj["playlists"].toArray();
    playlists.clear();
    
    for (const QJsonValue& value : playlistsArray) {
        QJsonObject playlistObj = value.toObject();
        Playlist playlist;
        playlist.name = playlistObj["name"].toString();
        
        QJsonArray videosArray = playlistObj["videos"].toArray();
        for (const QJsonValue& videoValue : videosArray) {
            QJsonObject videoObj = videoValue.toObject();
            VideoInfo video;
            video.videoId = videoObj["videoId"].toString();
            video.title = videoObj["title"].toString();
            video.channelTitle = videoObj["channelTitle"].toString();
            video.thumbnailUrl = videoObj["thumbnailUrl"].toString();
            video.description = videoObj["description"].toString();
            video.isFavorite = videoObj["isFavorite"].toBool();
            
            playlist.videos.append(video);
        }
        playlists.append(playlist);
    }
}

int Widget::getNextVideoIndex()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return -1;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.videos.isEmpty()) return -1;
    
    if (isShuffleMode) {
        return getRandomVideoIndex(true);
    } else {
        int newIndex = currentVideoIndex + 1;
        if (newIndex >= playlist.videos.size()) {
            if (isRepeatMode) {
                return 0;
            } else {
                return -1;
            }
        }
        return newIndex;
    }
}

int Widget::getRandomVideoIndex(bool excludeCurrent)
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return -1;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.videos.isEmpty()) return -1;
    
    if (playlist.videos.size() == 1) {
        if (excludeCurrent && currentVideoIndex == 0) {
            return -1;
        }
        return 0;
    }
    
    QList<int> unplayedVideos = getUnplayedVideoIndices(excludeCurrent);
    
    if (unplayedVideos.isEmpty() && isRepeatMode) {
        playedVideosInCurrentSession.clear();
        unplayedVideos = getUnplayedVideoIndices(excludeCurrent);
    }
    
    if (unplayedVideos.isEmpty()) {
        return -1;
    }
    
    int randomIndex = QRandomGenerator::global()->bounded(unplayedVideos.size());
    return unplayedVideos[randomIndex];
}

QList<int> Widget::getUnplayedVideoIndices(bool excludeCurrent)
{
    QList<int> unplayedVideos;
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) {
        return unplayedVideos;
    }
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    
    for (int i = 0; i < playlist.videos.size(); i++) {
        if (!playedVideosInCurrentSession.contains(i)) {
            if (!excludeCurrent || i != currentVideoIndex) {
                unplayedVideos.append(i);
            }
        }
    }
    
    return unplayedVideos;
}
