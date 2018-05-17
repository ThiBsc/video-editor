#include "playercontrol.h"

#include <QSlider>
#include <QToolButton>
#include <QStyle>
#include <QLabel>
#include <QTime>
#include <QVBoxLayout>
#include <QHBoxLayout>

PlayerControl::PlayerControl(QWidget *parent)
    : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    positionSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setMaximum(100);
    volumeSlider->setMinimum(0);
    volumeSlider->setValue(50);

    btnPlay = new QToolButton(this);
    btnStop = new QToolButton(this);
    btnPause = new QToolButton(this);
    btnPlay->setText("Play");
    btnStop->setText("Stop");
    btnPause->setText("Pause");
    btnPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    btnStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    btnPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));

    lblPosition = new QLabel(QString("%1/%1").arg(QTime(0, 0, 0).toString()), this);
    lblVolume = new QLabel(QString("%1%").arg(QString::number(volumeSlider->value())), this);

    hTopLayout = new QHBoxLayout();
    hTopLayout->setContentsMargins(0, 0, 0, 0);
    hTopLayout->addWidget(positionSlider);
    hTopLayout->addWidget(lblPosition);

    hBottomLayout = new QHBoxLayout();
    hBottomLayout->setContentsMargins(0, 0, 0, 0);
    hBottomLayout->addWidget(btnPlay);
    hBottomLayout->addWidget(btnStop);
    hBottomLayout->addWidget(btnPause);
    hBottomLayout->addWidget(volumeSlider);
    hBottomLayout->addWidget(lblVolume);

    mainLayout->addLayout(hTopLayout);
    mainLayout->addLayout(hBottomLayout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    connect(btnPlay, SIGNAL(released()), this, SLOT(emitPlayClicked()));
    connect(btnStop, SIGNAL(released()), this, SLOT(emitStopClicked()));
    connect(btnPause, SIGNAL(released()), this, SLOT(emitPauseClicked()));
    connect(volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(emitVolumeChanged(int)));
    connect(positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(emitPositionChanged(int)));
}

PlayerControl::~PlayerControl()
{
    delete positionSlider;
    delete volumeSlider;
    delete lblPosition;
    delete lblVolume;

    delete btnPlay;
    delete btnStop;
    delete btnPause;

    delete hTopLayout;
    delete hBottomLayout;
    delete mainLayout;
}

/**
 * @brief PlayerControl::getVolume
 * @return The sound volume in %
 */
int PlayerControl::getVolume()
{
    return volumeSlider->value();
}

/**
 * @brief PlayerControl::updateDuration
 * @param duration
 * Set the max duration value to the positionSlider
 */
void PlayerControl::updateDuration(qint64 duration)
{
    positionSlider->setMaximum(duration);

    int seconds = (duration/1000) % 60;
    int minutes = (duration/60000) % 60;
    int hours = (duration/3600000) % 24;
    QTime time(hours, minutes, seconds);
    QTime ctime(0, 0, 0);
    lblPosition->setText(ctime.toString()+"/"+time.toString());
}

/**
 * @brief PlayerControl::updateCursorPosition
 * @param position
 * Update the current cursor position to the positionSlider
 */
void PlayerControl::updateCursorPosition(qint64 position)
{
    positionSlider->setValue(position);

    int duration = positionSlider->maximum();
    int seconds = (duration/1000) % 60;
    int minutes = (duration/60000) % 60;
    int hours = (duration/3600000) % 24;
    QTime time(hours, minutes, seconds);
    int cseconds = (position/1000) % 60;
    int cminutes = (position/60000) % 60;
    int chours = (position/3600000) % 24;
    QTime ctime(chours, cminutes, cseconds);
    lblPosition->setText(ctime.toString()+"/"+time.toString());
}

/**
 * @brief PlayerControl::emitVolumeChanged
 * @param vol
 * Emit the new volume value
 */
void PlayerControl::emitVolumeChanged(int vol)
{
    lblVolume->setText(QString("%1%").arg(vol));
    emit volumeChanged(vol);
}

/**
 * @brief PlayerControl::emitPositionChanged
 * @param pos
 * Emit the new current cursor position
 */
void PlayerControl::emitPositionChanged(int pos)
{
    emit positionChanged(pos);
}

/**
 * @brief PlayerControl::emitPlayClicked
 * Emit signal "playClicked()"
 */
void PlayerControl::emitPlayClicked()
{
    emit playClicked();
}

/**
 * @brief PlayerControl::emitStopClicked
 * Emit signal "stopClicked()"
 */
void PlayerControl::emitStopClicked()
{
    emit stopClicked();
}

/**
 * @brief PlayerControl::emitPauseClicked
 * Emit signal "pauseClicked()"
 */
void PlayerControl::emitPauseClicked()
{
    emit pauseClicked();
}
