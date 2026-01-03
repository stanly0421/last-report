#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QListWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QLineEdit>
#include <QInputDialog>
#include <QMessageBox>
#include <QTime>
#include <QUrl>
#include <QMap>
#include <QList>
#include <QSet>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

// 影片/音樂資訊結構
struct VideoInfo {
    QString videoId;          // YouTube 影片 ID (用於 YouTube 連結)
    QString filePath;         // 本地檔案路徑 (用於本地音樂)
    QString title;            // 影片/音樂標題
    QString channelTitle;     // 頻道名稱/藝術家
    QString thumbnailUrl;     // 縮圖 URL
    QString description;      // 描述
    bool isFavorite;          // 是否為喜愛的影片/音樂
    bool isLocalFile;         // 是否為本地檔案
};

// 播放清單結構
struct Playlist {
    QString name;              // 播放清單名稱
    QList<VideoInfo> videos;   // 影片列表
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    // 播放控制
    void onPlayPauseClicked();
    void onPreviousClicked();
    void onNextClicked();
    void onShuffleClicked();
    void onRepeatClicked();
    
    // 搜尋功能
    void onSearchClicked();
    void onLoadLocalFileClicked();
    
    // 播放清單管理
    void onVideoDoubleClicked(QListWidgetItem* item);
    void onToggleFavoriteClicked();
    
    // 播放清單選擇
    void onNewPlaylistClicked();
    void onDeletePlaylistClicked();
    void onPlaylistChanged(int index);
    
    // 媒體播放器
    void onMediaPlayerStateChanged();
    void onMediaPlayerPositionChanged(qint64 position);
    void onMediaPlayerDurationChanged(qint64 duration);

private:
    void setupUI();
    void createConnections();
    void updatePlaylistDisplay();
    void playVideo(int index);
    void updateButtonStates();
    void savePlaylistsToFile();
    void loadPlaylistsFromFile();
    int getNextVideoIndex();
    int getRandomVideoIndex(bool excludeCurrent = true);
    QList<int> getUnplayedVideoIndices(bool excludeCurrent = true);
    void playYouTubeLink(const QString& link);
    void playLocalFile(const QString& filePath);
    QString extractYouTubeVideoId(const QString& url);
    QString createVideoDisplayHTML(const VideoInfo& video);

    Ui::Widget *ui;
    
    // 媒體播放器
    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;
    
    // 影片資訊顯示區域
    QLabel* videoDisplayLabel;
    
    // UI 元件
    QLineEdit* searchEdit;
    QPushButton* searchButton;
    QPushButton* loadLocalFileButton;
    QLabel* videoTitleLabel;
    QLabel* channelLabel;
    QPushButton* playPauseButton;
    QPushButton* previousButton;
    QPushButton* nextButton;
    QPushButton* shuffleButton;
    QPushButton* repeatButton;
    QPushButton* toggleFavoriteButton;
    QPushButton* newPlaylistButton;
    QPushButton* deletePlaylistButton;
    QListWidget* playlistWidget;
    QComboBox* playlistComboBox;
    
    // 播放清單數據
    QList<Playlist> playlists;
    int currentPlaylistIndex;
    int currentVideoIndex;
    bool isShuffleMode;
    bool isRepeatMode;
    bool isPlaying;
    QString lastPlaylistName;
    QSet<int> playedVideosInCurrentSession;
};

#endif // WIDGET_H
