#include "widget.h"
#include "ui_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPixmap>
#include <QImage>
#include <QBuffer>
#include <QFile>
#include <QDir>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , player(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput(this))
    , currentPlaylistIndex(-1)
    , currentSongIndex(-1)
    , isSliderBeingDragged(false)
{
    ui->setupUi(this);
    
    // 設置音頻輸出
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.5);
    
    // 設置窗口
    setWindowTitle("音樂播放器 Music Player");
    setMinimumSize(800, 600);
    
    // 建立UI
    setupUI();
    
    // 建立信號連接
    createConnections();
    
    // 創建默認播放清單
    Playlist defaultPlaylist;
    defaultPlaylist.name = "默認播放清單";
    playlists.append(defaultPlaylist);
    playlistComboBox->addItem(defaultPlaylist.name);
    currentPlaylistIndex = 0;
    
    // 更新按鈕狀態
    updateButtonStates();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setupUI()
{
    // 主佈局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // === 上半部分：封面和播放信息 ===
    QHBoxLayout* topLayout = new QHBoxLayout();
    
    // 封面區域
    QVBoxLayout* coverLayout = new QVBoxLayout();
    coverLabel = new QLabel(this);
    coverLabel->setFixedSize(200, 200);
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setStyleSheet(
        "QLabel {"
        "   background-color: #333;"
        "   border: 2px solid #555;"
        "   border-radius: 10px;"
        "   color: #888;"
        "}"
    );
    coverLabel->setText("無封面\nNo Cover");
    coverLayout->addWidget(coverLabel);
    coverLayout->addStretch();
    topLayout->addLayout(coverLayout);
    
    // 播放信息區域
    QVBoxLayout* infoLayout = new QVBoxLayout();
    
    songTitleLabel = new QLabel("未選擇歌曲", this);
    songTitleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #333;");
    songTitleLabel->setWordWrap(true);
    infoLayout->addWidget(songTitleLabel);
    
    artistLabel = new QLabel("", this);
    artistLabel->setStyleSheet("font-size: 14px; color: #666;");
    infoLayout->addWidget(artistLabel);
    
    infoLayout->addStretch();
    
    // 進度條區域
    QVBoxLayout* progressLayout = new QVBoxLayout();
    
    progressSlider = new QSlider(Qt::Horizontal, this);
    progressSlider->setRange(0, 0);
    progressSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   border: 1px solid #999999;"
        "   height: 8px;"
        "   background: #E0E0E0;"
        "   margin: 2px 0;"
        "   border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #4CAF50;"
        "   border: 1px solid #5c5c5c;"
        "   width: 18px;"
        "   margin: -5px 0;"
        "   border-radius: 9px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "   background: #4CAF50;"
        "   border-radius: 4px;"
        "}"
    );
    progressLayout->addWidget(progressSlider);
    
    timeLabel = new QLabel("00:00 / 00:00", this);
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setStyleSheet("font-size: 12px; color: #666;");
    progressLayout->addWidget(timeLabel);
    
    infoLayout->addLayout(progressLayout);
    topLayout->addLayout(infoLayout, 1);
    
    mainLayout->addLayout(topLayout);
    
    // === 播放控制區域 ===
    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(10);
    
    QString buttonStyle = 
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   padding: 10px 20px;"
        "   font-size: 14px;"
        "   border-radius: 5px;"
        "   min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #cccccc;"
        "   color: #666666;"
        "}";
    
    previousButton = new QPushButton("⏮ 上一首", this);
    previousButton->setStyleSheet(buttonStyle);
    controlLayout->addWidget(previousButton);
    
    playPauseButton = new QPushButton("▶ 播放", this);
    playPauseButton->setStyleSheet(buttonStyle);
    controlLayout->addWidget(playPauseButton);
    
    stopButton = new QPushButton("⏹ 停止", this);
    stopButton->setStyleSheet(buttonStyle);
    controlLayout->addWidget(stopButton);
    
    nextButton = new QPushButton("⏭ 下一首", this);
    nextButton->setStyleSheet(buttonStyle);
    controlLayout->addWidget(nextButton);
    
    controlLayout->addStretch();
    
    // 音量控制
    QLabel* volumeIconLabel = new QLabel("🔊", this);
    controlLayout->addWidget(volumeIconLabel);
    
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setFixedWidth(120);
    volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   border: 1px solid #999999;"
        "   height: 6px;"
        "   background: #E0E0E0;"
        "   margin: 2px 0;"
        "   border-radius: 3px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #2196F3;"
        "   border: 1px solid #5c5c5c;"
        "   width: 14px;"
        "   margin: -4px 0;"
        "   border-radius: 7px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "   background: #2196F3;"
        "   border-radius: 3px;"
        "}"
    );
    controlLayout->addWidget(volumeSlider);
    
    volumeLabel = new QLabel("50%", this);
    volumeLabel->setFixedWidth(40);
    controlLayout->addWidget(volumeLabel);
    
    mainLayout->addLayout(controlLayout);
    
    // === 下半部分：播放清單管理 ===
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    
    // 播放清單選擇器
    QGroupBox* playlistGroupBox = new QGroupBox("播放清單管理", this);
    QVBoxLayout* playlistGroupLayout = new QVBoxLayout(playlistGroupBox);
    
    QHBoxLayout* playlistSelectorLayout = new QHBoxLayout();
    playlistComboBox = new QComboBox(this);
    playlistComboBox->setMinimumWidth(200);
    playlistSelectorLayout->addWidget(playlistComboBox);
    
    newPlaylistButton = new QPushButton("新增", this);
    newPlaylistButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   padding: 5px 15px;"
        "   font-size: 12px;"
        "   border-radius: 3px;"
        "}"
        "QPushButton:hover { background-color: #1976D2; }"
    );
    playlistSelectorLayout->addWidget(newPlaylistButton);
    
    deletePlaylistButton = new QPushButton("刪除", this);
    deletePlaylistButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   border: none;"
        "   padding: 5px 15px;"
        "   font-size: 12px;"
        "   border-radius: 3px;"
        "}"
        "QPushButton:hover { background-color: #d32f2f; }"
    );
    playlistSelectorLayout->addWidget(deletePlaylistButton);
    
    playlistGroupLayout->addLayout(playlistSelectorLayout);
    
    // 歌曲列表
    playlistWidget = new QListWidget(this);
    playlistWidget->setStyleSheet(
        "QListWidget {"
        "   border: 1px solid #ccc;"
        "   border-radius: 5px;"
        "   background-color: #f9f9f9;"
        "}"
        "QListWidget::item {"
        "   padding: 8px;"
        "   border-bottom: 1px solid #eee;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #e8f5e9;"
        "}"
    );
    playlistGroupLayout->addWidget(playlistWidget);
    
    // 歌曲管理按鈕
    QHBoxLayout* songButtonLayout = new QHBoxLayout();
    
    QString smallButtonStyle = 
        "QPushButton {"
        "   background-color: #607D8B;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 15px;"
        "   font-size: 12px;"
        "   border-radius: 3px;"
        "}"
        "QPushButton:hover { background-color: #546E7A; }"
        "QPushButton:disabled { background-color: #cccccc; color: #666666; }";
    
    addSongsButton = new QPushButton("添加歌曲", this);
    addSongsButton->setStyleSheet(smallButtonStyle);
    songButtonLayout->addWidget(addSongsButton);
    
    removeSongButton = new QPushButton("移除歌曲", this);
    removeSongButton->setStyleSheet(smallButtonStyle);
    songButtonLayout->addWidget(removeSongButton);
    
    moveUpButton = new QPushButton("上移", this);
    moveUpButton->setStyleSheet(smallButtonStyle);
    songButtonLayout->addWidget(moveUpButton);
    
    moveDownButton = new QPushButton("下移", this);
    moveDownButton->setStyleSheet(smallButtonStyle);
    songButtonLayout->addWidget(moveDownButton);
    
    playlistGroupLayout->addLayout(songButtonLayout);
    
    bottomLayout->addWidget(playlistGroupBox);
    
    mainLayout->addLayout(bottomLayout, 1);
}

void Widget::createConnections()
{
    // 播放器信號
    connect(player, &QMediaPlayer::positionChanged, this, &Widget::onPositionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &Widget::onDurationChanged);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &Widget::onMediaStatusChanged);
    connect(player, &QMediaPlayer::playbackStateChanged, this, &Widget::onPlaybackStateChanged);
    
    // 播放控制按鈕
    connect(playPauseButton, &QPushButton::clicked, this, &Widget::onPlayPauseClicked);
    connect(stopButton, &QPushButton::clicked, this, &Widget::onStopClicked);
    connect(previousButton, &QPushButton::clicked, this, &Widget::onPreviousClicked);
    connect(nextButton, &QPushButton::clicked, this, &Widget::onNextClicked);
    
    // 進度條
    connect(progressSlider, &QSlider::sliderMoved, this, &Widget::onProgressSliderMoved);
    connect(progressSlider, &QSlider::sliderPressed, this, &Widget::onProgressSliderPressed);
    connect(progressSlider, &QSlider::sliderReleased, this, &Widget::onProgressSliderReleased);
    
    // 音量
    connect(volumeSlider, &QSlider::valueChanged, this, &Widget::onVolumeChanged);
    
    // 播放清單管理
    connect(addSongsButton, &QPushButton::clicked, this, &Widget::onAddSongsClicked);
    connect(removeSongButton, &QPushButton::clicked, this, &Widget::onRemoveSongClicked);
    connect(moveUpButton, &QPushButton::clicked, this, &Widget::onMoveUpClicked);
    connect(moveDownButton, &QPushButton::clicked, this, &Widget::onMoveDownClicked);
    connect(playlistWidget, &QListWidget::itemDoubleClicked, this, &Widget::onSongDoubleClicked);
    
    // 播放清單選擇
    connect(newPlaylistButton, &QPushButton::clicked, this, &Widget::onNewPlaylistClicked);
    connect(deletePlaylistButton, &QPushButton::clicked, this, &Widget::onDeletePlaylistClicked);
    connect(playlistComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Widget::onPlaylistChanged);
}

void Widget::onPlayPauseClicked()
{
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
    } else if (player->playbackState() == QMediaPlayer::PausedState) {
        player->play();
    } else {
        // 如果沒有正在播放的歌曲，播放當前選中的歌曲或第一首
        if (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size()) {
            Playlist& playlist = playlists[currentPlaylistIndex];
            if (!playlist.songs.isEmpty()) {
                int index = playlistWidget->currentRow();
                if (index < 0) index = 0;
                playSong(index);
            }
        }
    }
}

void Widget::onStopClicked()
{
    player->stop();
    progressSlider->setValue(0);
    updateTimeDisplay(0, player->duration());
}

void Widget::onPreviousClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.songs.isEmpty()) return;
    
    int newIndex = currentSongIndex - 1;
    if (newIndex < 0) {
        newIndex = playlist.songs.size() - 1; // 循環到最後一首
    }
    playSong(newIndex);
}

void Widget::onNextClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (playlist.songs.isEmpty()) return;
    
    int newIndex = currentSongIndex + 1;
    if (newIndex >= playlist.songs.size()) {
        newIndex = 0; // 循環到第一首
    }
    playSong(newIndex);
}

void Widget::onPositionChanged(qint64 position)
{
    if (!isSliderBeingDragged) {
        progressSlider->setValue(static_cast<int>(position));
    }
    updateTimeDisplay(position, player->duration());
}

void Widget::onDurationChanged(qint64 duration)
{
    progressSlider->setRange(0, static_cast<int>(duration));
    updateTimeDisplay(player->position(), duration);
}

void Widget::onProgressSliderMoved(int position)
{
    updateTimeDisplay(position, player->duration());
}

void Widget::onProgressSliderPressed()
{
    isSliderBeingDragged = true;
}

void Widget::onProgressSliderReleased()
{
    isSliderBeingDragged = false;
    player->setPosition(progressSlider->value());
}

void Widget::onVolumeChanged(int value)
{
    audioOutput->setVolume(value / 100.0f);
    volumeLabel->setText(QString("%1%").arg(value));
}

void Widget::onAddSongsClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        "選擇音樂檔案",
        QDir::homePath(),
        "音頻檔案 (*.mp3 *.wav *.flac *.aac *.ogg *.m4a *.wma);;所有檔案 (*.*)"
    );
    
    if (files.isEmpty()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    for (const QString& file : files) {
        SongInfo info = extractSongInfo(file);
        playlist.songs.append(info);
    }
    
    updatePlaylistDisplay();
    updateButtonStates();
}

void Widget::onRemoveSongClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    int selectedRow = playlistWidget->currentRow();
    if (selectedRow < 0) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (selectedRow < playlist.songs.size()) {
        // 如果刪除的是當前播放的歌曲，停止播放
        if (selectedRow == currentSongIndex) {
            player->stop();
            currentSongIndex = -1;
        } else if (selectedRow < currentSongIndex) {
            currentSongIndex--;
        }
        
        playlist.songs.removeAt(selectedRow);
        updatePlaylistDisplay();
        updateButtonStates();
    }
}

void Widget::onMoveUpClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    int selectedRow = playlistWidget->currentRow();
    if (selectedRow <= 0) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (selectedRow < playlist.songs.size()) {
        playlist.songs.swapItemsAt(selectedRow, selectedRow - 1);
        
        // 更新當前歌曲索引
        if (currentSongIndex == selectedRow) {
            currentSongIndex = selectedRow - 1;
        } else if (currentSongIndex == selectedRow - 1) {
            currentSongIndex = selectedRow;
        }
        
        updatePlaylistDisplay();
        playlistWidget->setCurrentRow(selectedRow - 1);
    }
}

void Widget::onMoveDownClicked()
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    int selectedRow = playlistWidget->currentRow();
    if (selectedRow < 0 || selectedRow >= playlist.songs.size() - 1) return;
    
    playlist.songs.swapItemsAt(selectedRow, selectedRow + 1);
    
    // 更新當前歌曲索引
    if (currentSongIndex == selectedRow) {
        currentSongIndex = selectedRow + 1;
    } else if (currentSongIndex == selectedRow + 1) {
        currentSongIndex = selectedRow;
    }
    
    updatePlaylistDisplay();
    playlistWidget->setCurrentRow(selectedRow + 1);
}

void Widget::onSongDoubleClicked(QListWidgetItem* item)
{
    int index = playlistWidget->row(item);
    playSong(index);
}

void Widget::onNewPlaylistClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新增播放清單", 
                                         "請輸入播放清單名稱:", 
                                         QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        Playlist newPlaylist;
        newPlaylist.name = name;
        playlists.append(newPlaylist);
        playlistComboBox->addItem(name);
        playlistComboBox->setCurrentIndex(playlists.size() - 1);
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
        player->stop();
        currentSongIndex = -1;
        playlists.removeAt(currentPlaylistIndex);
        playlistComboBox->removeItem(currentPlaylistIndex);
    }
}

void Widget::onPlaylistChanged(int index)
{
    if (index < 0 || index >= playlists.size()) return;
    
    currentPlaylistIndex = index;
    currentSongIndex = -1;
    updatePlaylistDisplay();
    updateButtonStates();
}

void Widget::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        // 自動播放下一首
        onNextClicked();
    }
}

void Widget::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    switch (state) {
    case QMediaPlayer::PlayingState:
        playPauseButton->setText("⏸ 暫停");
        break;
    case QMediaPlayer::PausedState:
        playPauseButton->setText("▶ 繼續");
        break;
    case QMediaPlayer::StoppedState:
        playPauseButton->setText("▶ 播放");
        break;
    }
}

void Widget::updatePlaylistDisplay()
{
    playlistWidget->clear();
    
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    const Playlist& playlist = playlists[currentPlaylistIndex];
    for (int i = 0; i < playlist.songs.size(); i++) {
        const SongInfo& song = playlist.songs[i];
        QString displayText = QString("%1. %2").arg(i + 1).arg(song.title);
        if (!song.artist.isEmpty()) {
            displayText += QString(" - %1").arg(song.artist);
        }
        
        QListWidgetItem* item = new QListWidgetItem(displayText);
        
        // 高亮當前播放的歌曲
        if (i == currentSongIndex) {
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
            item->setBackground(QColor("#e8f5e9"));
        }
        
        playlistWidget->addItem(item);
    }
}

void Widget::playSong(int index)
{
    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= playlists.size()) return;
    
    Playlist& playlist = playlists[currentPlaylistIndex];
    if (index < 0 || index >= playlist.songs.size()) return;
    
    currentSongIndex = index;
    const SongInfo& song = playlist.songs[index];
    
    // 設置媒體源
    player->setSource(QUrl::fromLocalFile(song.filePath));
    player->play();
    
    // 更新顯示
    songTitleLabel->setText(song.title);
    artistLabel->setText(song.artist);
    updateCoverArt(song.filePath);
    updatePlaylistDisplay();
    
    // 選中當前歌曲
    playlistWidget->setCurrentRow(index);
}

void Widget::updateCoverArt(const QString& filePath)
{
    // 嘗試從文件元數據讀取封面
    // TODO: 完整的實現需要使用 TagLib 或 FFmpeg 等庫來讀取 ID3 標籤中的封面
    // 當前實現：搜尋同目錄下常見的封面圖片文件
    
    QPixmap defaultCover(200, 200);
    defaultCover.fill(QColor("#333"));
    
    // 嘗試查找同目錄下的封面圖片
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.dir();
    QStringList coverFiles = {"cover.jpg", "cover.png", "folder.jpg", "folder.png", 
                              "album.jpg", "album.png", "artwork.jpg", "artwork.png"};
    
    for (const QString& coverFile : coverFiles) {
        QString coverPath = dir.filePath(coverFile);
        if (QFile::exists(coverPath)) {
            QPixmap cover(coverPath);
            if (!cover.isNull()) {
                coverLabel->setPixmap(cover.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                return;
            }
        }
    }
    
    // 如果沒有找到封面，顯示默認圖片
    coverLabel->setPixmap(defaultCover);
    coverLabel->setText("無封面\nNo Cover");
}

void Widget::updateTimeDisplay(qint64 position, qint64 duration)
{
    QString posStr = formatTime(position);
    QString durStr = formatTime(duration);
    timeLabel->setText(QString("%1 / %2").arg(posStr).arg(durStr));
}

QString Widget::formatTime(qint64 ms)
{
    int seconds = static_cast<int>(ms / 1000);
    int minutes = seconds / 60;
    seconds = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

SongInfo Widget::extractSongInfo(const QString& filePath)
{
    SongInfo info;
    info.filePath = filePath;
    info.duration = 0;
    
    QFileInfo fileInfo(filePath);
    info.title = fileInfo.completeBaseName();
    info.artist = "";
    
    // 嘗試從文件名解析藝術家信息 (格式: "藝術家 - 歌曲名")
    QString baseName = fileInfo.completeBaseName();
    int dashIndex = baseName.indexOf(" - ");
    if (dashIndex != -1) {
        info.artist = baseName.left(dashIndex).trimmed();
        info.title = baseName.mid(dashIndex + 3).trimmed();
    }
    
    return info;
}

void Widget::saveCurrentPlaylist()
{
    // TODO: 實現播放清單保存到文件的功能
    // 可以使用 JSON 或 XML 格式保存播放清單數據
}

void Widget::loadPlaylist(int /* index */)
{
    // TODO: 實現從文件加載播放清單的功能
    // 可以使用 JSON 或 XML 格式讀取播放清單數據
}

void Widget::updateButtonStates()
{
    bool hasPlaylist = (currentPlaylistIndex >= 0 && currentPlaylistIndex < playlists.size());
    bool hasSongs = hasPlaylist && !playlists[currentPlaylistIndex].songs.isEmpty();
    int selectedRow = playlistWidget->currentRow();
    bool hasSelection = selectedRow >= 0;
    
    playPauseButton->setEnabled(hasSongs);
    stopButton->setEnabled(hasSongs);
    previousButton->setEnabled(hasSongs);
    nextButton->setEnabled(hasSongs);
    removeSongButton->setEnabled(hasSelection);
    moveUpButton->setEnabled(hasSelection && selectedRow > 0);
    moveDownButton->setEnabled(hasSelection && hasSongs && selectedRow < playlists[currentPlaylistIndex].songs.size() - 1);
    deletePlaylistButton->setEnabled(playlists.size() > 1);
}
