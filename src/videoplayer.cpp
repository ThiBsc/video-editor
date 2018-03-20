#include "videoplayer.h"
#include "playercontrol.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QVBoxLayout>
#include <QMessageBox>

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
    //connect(mediaPlayer, SIGNAL(durationChanged(qint64)), playerControl, SLOT(updateMaxDuration(qint64)));
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(totalDurationIsAboutToBeChanged(qint64)));
    //connect(mediaPlayer, SIGNAL(positionChanged(qint64)), playerControl, SLOT(updateCursorPosition(qint64)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(updateCursorPosition(qint64)));

    mediaPlayer->setVolume(playerControl->getVolume());
    total_duration = 0;
    duration_count = 0;
}

VideoPlayer::~VideoPlayer()
{
    delete videoWidget;
    delete mediaPlaylist;
    delete mediaPlayer;
    delete playerControl;
    delete vLayout;
}

void VideoPlayer::addRush(QList<QMediaContent> rush)
{
    bool added = mediaPlaylist->addMedia(rush);
    if (added){
        playerControl->blockSignals(true);
        for (QMediaContent content : rush){
            mediaPlayer->setMedia(content);
        }
        mediaPlayer->setPlaylist(mediaPlaylist);
        playerControl->blockSignals(false);
    }
}

void VideoPlayer::totalDurationIsAboutToBeChanged(qint64 duration)
{
    if (duration_count != mediaPlaylist->mediaCount()){
        contentDuration.append(duration);
        total_duration += duration;
        playerControl->updateMaxDuration(total_duration);
        duration_count++;
    }
}

void VideoPlayer::updateCursorPosition(qint64 position)
{
    int cur_idx = mediaPlaylist->currentIndex();
    qint64 current_position = 0;
    for (int i=0; i<cur_idx; i++)
        current_position += contentDuration.at(i);
    playerControl->updateCursorPosition(current_position+position);
}
