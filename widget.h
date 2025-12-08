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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebEngineView>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

// 影片資訊結構
struct VideoInfo {
    QString videoId;          // YouTube 影片 ID
    QString title;            // 影片標題
    QString channelTitle;     // 頻道名稱
    QString thumbnailUrl;     // 縮圖 URL
    QString description;      // 描述
    bool isFavorite;          // 是否為喜愛的影片
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
    void onSearchFinished();
    
    // 播放清單管理
    void onAddToPlaylistClicked();
    void onRemoveVideoClicked();
    void onVideoDoubleClicked(QListWidgetItem* item);
    void onToggleFavoriteClicked();
    
    // 播放清單選擇
    void onNewPlaylistClicked();
    void onDeletePlaylistClicked();
    void onPlaylistChanged(int index);
    
    // 網路請求
    void onNetworkReplyFinished(QNetworkReply* reply);

private:
    void setupUI();
    void createConnections();
    void updatePlaylistDisplay();
    void playVideo(int index);
    void updateButtonStates();
    void savePlaylistsToFile();
    void loadPlaylistsFromFile();
    void updateNextVideoDisplay();
    int getNextVideoIndex();
    int getRandomVideoIndex(bool excludeCurrent = true);
    QList<int> getUnplayedVideoIndices(bool excludeCurrent = true);
    void searchYouTube(const QString& query);
    void showSearchResults(const QJsonArray& items);

    Ui::Widget *ui;
    
    // YouTube API
    QNetworkAccessManager* networkManager;
    QString apiKey;
    QList<VideoInfo> searchResults;
    
    // 網頁播放器
    QWebEngineView* webEngineView;
    
    // UI 元件
    QLineEdit* searchEdit;
    QPushButton* searchButton;
    QListWidget* searchResultsWidget;
    QLabel* videoTitleLabel;
    QLabel* channelLabel;
    QPushButton* playPauseButton;
    QPushButton* previousButton;
    QPushButton* nextButton;
    QPushButton* shuffleButton;
    QPushButton* repeatButton;
    QPushButton* addToPlaylistButton;
    QPushButton* removeVideoButton;
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
