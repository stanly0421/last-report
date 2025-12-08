#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QListWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QTime>
#include <QUrl>
#include <QMap>
#include <QList>
#include <QSet>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

// 歌曲資訊結構
struct SongInfo {
    QString filePath;     // 檔案路徑
    QString title;        // 歌曲標題
    QString artist;       // 藝術家
    qint64 duration;      // 時長（毫秒）
    QByteArray coverData; // 封面數據
};

// 播放清單結構
struct Playlist {
    QString name;              // 播放清單名稱
    QList<SongInfo> songs;     // 歌曲列表
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
    
    // 進度控制
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onProgressSliderMoved(int position);
    void onProgressSliderPressed();
    void onProgressSliderReleased();
    
    // 音量控制
    void onVolumeChanged(int value);
    
    // 播放清單管理
    void onAddSongsClicked();
    void onRemoveSongClicked();
    void onSongDoubleClicked(QListWidgetItem* item);
    
    // 播放清單選擇
    void onNewPlaylistClicked();
    void onDeletePlaylistClicked();
    void onPlaylistChanged(int index);
    
    // 封面上傳
    void onUploadCoverClicked();
    
    // 播放器狀態
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);

private:
    void setupUI();
    void createConnections();
    void updatePlaylistDisplay();
    void playSong(int index);
    void updateCoverArt(const QString& filePath);
    void updateTimeDisplay(qint64 position, qint64 duration);
    QString formatTime(qint64 ms);
    SongInfo extractSongInfo(const QString& filePath);
    void saveCurrentPlaylist();
    void loadPlaylist(int index);
    void updateButtonStates();
    void savePlaylistsToFile();
    void loadPlaylistsFromFile();
    void updateNextSongDisplay();
    int getNextSongIndex();
    int getRandomSongIndex(bool excludeCurrent = true);
    void saveCustomCover(const QString& songPath, const QString& coverPath);
    QString getCustomCoverPath(const QString& songPath);

    Ui::Widget *ui;
    
    // 媒體播放器
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    
    // UI 元件
    QLabel* coverLabel;
    QLabel* songTitleLabel;
    QLabel* artistLabel;
    QLabel* nextSongLabel;
    QLabel* timeLabel;
    QSlider* progressSlider;
    QSlider* volumeSlider;
    QLabel* volumeLabel;
    QPushButton* playPauseButton;
    QPushButton* previousButton;
    QPushButton* nextButton;
    QPushButton* shuffleButton;
    QPushButton* repeatButton;
    QPushButton* addSongsButton;
    QPushButton* removeSongButton;
    QPushButton* newPlaylistButton;
    QPushButton* deletePlaylistButton;
    QPushButton* uploadCoverButton;
    QListWidget* playlistWidget;
    QComboBox* playlistComboBox;
    
    // 播放清單數據
    QList<Playlist> playlists;
    int currentPlaylistIndex;
    int currentSongIndex;
    bool isSliderBeingDragged;
    bool isShuffleMode;
    bool isRepeatMode;
    QString lastPlaylistName;
    QSet<int> playedSongsInCurrentSession;
};

#endif // WIDGET_H
