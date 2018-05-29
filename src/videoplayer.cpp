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
    isReady = false;
    durationIsNeeded = false;
    lastIndex = -1;
    vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(vLayout);

    mediaPlayer = new QMediaPlayer(this);
    mediaPlaylist = new QMediaPlaylist(mediaPlayer);
    mediaPlaylist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
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
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), playerControl, SLOT(updateDuration(qint64)));
    connect(mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), playerControl, SLOT(playerStateChanged(QMediaPlayer::State)));
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(playerReady()));
    connect(mediaPlaylist, SIGNAL(currentIndexChanged(int)), this, SLOT(emitCurrentMediaChanged(int)));

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

void VideoPlayer::setPlayMode(QMediaPlaylist::PlaybackMode pbm)
{
    mediaPlaylist->setPlaybackMode(pbm);
}

void VideoPlayer::addMediaToPlaylist(Media &media)
{
    mediaPlaylist->addMedia(QMediaContent(QUrl::fromLocalFile(media.currentPath())));
}

void VideoPlayer::removeMediaToPlaylist(int i)
{
    mediaPlaylist->removeMedia(i);
}

void VideoPlayer::moveMediaInPlaylist(int a, int b)
{
    QMediaContent content_a = mediaPlaylist->media(a);
    mediaPlaylist->insertMedia(a < b ? b+1 : b, content_a);
    mediaPlaylist->removeMedia(a < b ? a : a+1);
}

void VideoPlayer::setCurrentMedia(int i, Media &media)
{
    Q_UNUSED(media);
    isReady = false;
    durationIsNeeded = false;
    mediaPlaylist->setCurrentIndex(i);
    lastIndex = i;
    //mediaPlaylist->addMedia(QMediaContent(QUrl::fromLocalFile(media.currentPath())));
    //playerControl->updateDuration(QTime(0, 0, 0).msecsTo(media.getDuration()));
}

void VideoPlayer::emitCurrentMediaChanged(int i)
{
    if (mediaPlaylist->playbackMode() == QMediaPlaylist::Sequential){
        emit currentMediaChanged(i);
    } else if (i == -1){
        mediaPlayer->stop();
        mediaPlaylist->setCurrentIndex(lastIndex);
    }
}

void VideoPlayer::timeAsked()
{
    durationIsNeeded = true;
    if (isReady){
        emit sendDuration(mediaPlayer->duration());
    }
}

void VideoPlayer::playerReady()
{
    if (durationIsNeeded){
        emit sendDuration(mediaPlayer->duration());
    }
}

