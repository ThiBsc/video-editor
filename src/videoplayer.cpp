#include "videoplayer.h"
#include "playercontrol.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTime>

VideoPlayer::VideoPlayer(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(vLayout);

    mediaPlayer = new QMediaPlayer(this);
    mediaPlaylist = new QMediaPlaylist(mediaPlayer);
    mediaPlaylist->setPlaybackMode(QMediaPlaylist::Sequential);
    mediaPlayer->setPlaylist(mediaPlaylist);
    videoWidget = new QVideoWidget(this);

    mediaPlayer->setVideoOutput(videoWidget);

    playerControl = new PlayerControl(this);

    vLayout->addWidget(videoWidget);
    vLayout->addWidget(playerControl);

    if (!mediaPlayer->isAvailable()) {
        QMessageBox::warning(this, tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"\
                                "Please check the media service plugins are installed."));

        playerControl->setEnabled(false);
    }

    connect(playerControl, SIGNAL(playClicked()), mediaPlayer, SLOT(play()));
    connect(playerControl, SIGNAL(stopClicked()), mediaPlayer, SLOT(stop()));
    connect(playerControl, SIGNAL(pauseClicked()), mediaPlayer, SLOT(pause()));
    connect(playerControl, SIGNAL(volumeChanged(int)), mediaPlayer, SLOT(setVolume(int)));
    connect(playerControl, SIGNAL(positionChanged(qint64)), mediaPlayer, SLOT(setPosition(qint64)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), playerControl, SLOT(updateCursorPosition(qint64)));

    mediaPlayer->setVolume(playerControl->getVolume());
}

VideoPlayer::~VideoPlayer()
{
    delete videoWidget;
    delete mediaPlaylist;
    delete mediaPlayer;
    delete playerControl;
    delete vLayout;
}

/**
 * @brief VideoPlayer::getPlayerControl
 * @return playerControl
 */
PlayerControl *VideoPlayer::getPlayerControl()
{
    return playerControl;
}

QMediaPlayer *VideoPlayer::getMediaPlayer()
{
    return mediaPlayer;
}

void VideoPlayer::setCurrentMedia(const Media &media)
{
    mediaPlayer->setMedia(QMediaContent(QUrl::fromLocalFile(media.currentPath())));
    playerControl->updateDuration(QTime(0, 0, 0).msecsTo(media.getDuration()));
}
