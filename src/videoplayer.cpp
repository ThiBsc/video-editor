#include "videoplayer.h"
#include "playercontrol.h"

#include <QMediaPlayer>
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
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), playerControl, SLOT(updateMaxDuration(qint64)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), playerControl, SLOT(updateCursorPosition(qint64)));

    mediaPlayer->setMedia(QUrl::fromLocalFile("test.mkv"));
    mediaPlayer->setVolume(playerControl->getVolume());
}

VideoPlayer::~VideoPlayer()
{
    delete videoWidget;
    delete mediaPlayer;
    delete playerControl;
    delete vLayout;
}
