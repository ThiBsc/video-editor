#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QMediaContent>

class QMediaPlayer;
class QMediaPlaylist;
class QVideoWidget;
class QVBoxLayout;
class PlayerControl;

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~VideoPlayer();

public slots:
    void addRush(QList<QMediaContent> rush);
    void totalDurationIsAboutToBeChanged(qint64 duration);
    void updateCursorPosition(qint64 position);

private:
    QList<qint64> contentDuration;
    qint64 total_duration;
    int duration_count;
    QVBoxLayout *vLayout;

    QMediaPlayer *mediaPlayer;
    QMediaPlaylist *mediaPlaylist;
    QVideoWidget *videoWidget;
    PlayerControl *playerControl;

};

#endif // VIDEOPLAYER_H
