#include "playercontrol.h"

#include <QSlider>
#include <QToolButton>
#include <QStyle>
#include <QLabel>
#include <QTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>

bool PlayerControl::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == positionSlider && event->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent *mouseEvent = reinterpret_cast<QMouseEvent *>(event);
        qint64 position = QStyle::sliderValueFromPosition(positionSlider->minimum(), positionSlider->maximum(), mouseEvent->x(), positionSlider->width());
        updateCursorPosition(position);
        emitPositionChanged(position);
    }
    return false;
}

PlayerControl::PlayerControl(QWidget *parent)
    : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    positionSlider = new QSlider(Qt::Horizontal, this);
    positionSlider->installEventFilter(this);
    positionSlider->setRange(0, 0);
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setMaximum(100);
    volumeSlider->setMinimum(0);
    volumeSlider->setValue(50);

    btnPlayPause = new QToolButton(this);
    btnStop = new QToolButton(this);
    btnPlayPause->setText(tr("Play/Break"));
    btnStop->setText(tr("Stop"));
    btnPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    btnPlayPause->setCheckable(true);
    btnStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

    lblPosition = new QLabel(QString("%1/%1").arg(QTime(0, 0, 0).toString()), this);
    lblVolume = new QLabel(QString("%1%").arg(QString::number(volumeSlider->value())), this);

    hTopLayout = new QHBoxLayout();
    hTopLayout->setContentsMargins(0, 0, 0, 0);
    hTopLayout->addWidget(positionSlider);
    hTopLayout->addWidget(lblPosition);

    hBottomLayout = new QHBoxLayout();
    hBottomLayout->setContentsMargins(0, 0, 0, 0);
    hBottomLayout->addWidget(btnPlayPause);
    hBottomLayout->addWidget(btnStop);
    hBottomLayout->addWidget(volumeSlider);
    hBottomLayout->addWidget(lblVolume);

    mainLayout->addLayout(hTopLayout);
    mainLayout->addLayout(hBottomLayout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    connect(btnPlayPause, SIGNAL(released()), this, SLOT(emitPlayPauseClicked()));
    connect(btnStop, SIGNAL(released()), this, SLOT(emitStopClicked()));
    connect(volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(emitVolumeChanged(int)));
    connect(positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(emitPositionChanged(int)));
}

PlayerControl::~PlayerControl()
{
    delete positionSlider;
    delete volumeSlider;
    delete lblPosition;
    delete lblVolume;

    delete btnPlayPause;
    delete btnStop;

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

void PlayerControl::enablePlayerControl(bool enabled)
{
    btnPlayPause->setEnabled(enabled);
    btnStop->setEnabled(enabled);
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
 * @brief PlayerControl::emitPlayPauseClicked
 * Emit play or pause signal
 */
void PlayerControl::emitPlayPauseClicked()
{
    if (btnPlayPause->isChecked()){
        btnPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        emit playClicked();
    } else {
        btnPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        emit pauseClicked();
    }
}

/**
 * @brief PlayerControl::emitStopClicked
 * Emit signal "stopClicked()"
 */
void PlayerControl::emitStopClicked()
{
    btnPlayPause->setChecked(false);
    btnPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    positionSlider->setValue(0);
    emit stopClicked();
}

