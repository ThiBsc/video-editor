#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>

class QMediaPlayer;
class QVideoWidget;
class QVBoxLayout;
class PlayerControl;

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~VideoPlayer();

private:
    QVBoxLayout *vLayout;

    QMediaPlayer *mediaPlayer;
    QVideoWidget *videoWidget;
    PlayerControl *playerControl;

};

#endif // VIDEOPLAYER_H
