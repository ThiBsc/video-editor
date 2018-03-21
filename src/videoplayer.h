#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QMediaContent>

class QMediaPlayer;
class QMediaPlaylist;
class QVideoWidget;
class QVBoxLayout;
class PlayerControl;
class MediaFileInfo;

/**
 * @brief The VideoPlayer class
 * A graphic class to connect the mediaPlayer with playerControl
 */
class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~VideoPlayer();
    int64_t getPlaylistDuration();

public slots:
    void addRush(QList<QMediaContent> rush);

private:
    std::vector<int64_t> allDuration;
    MediaFileInfo *mediaInfo;
    QVBoxLayout *vLayout;

    QMediaPlayer *mediaPlayer;
    QMediaPlaylist *mediaPlaylist;
    QVideoWidget *videoWidget;
    PlayerControl *playerControl;

};

#endif // VIDEOPLAYER_H
