#include "track.h"

#include <QPaintEvent>
#include <QPainter>

Track::Track(Track::TrackType type, QWidget *parent)
    : QWidget(parent)
{
    this->type = type;

    setMinimumHeight(50);
}

Track::~Track()
{

}

void Track::addMarker(int64_t ms)
{
    markers.insert(ms);
}

/**
 * @brief Track::paintEvent
 * @param e
 * http://doc.qt.io/qt-5/qwidget.html#paintEvent
 */
void Track::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    int w = width(), h = height();
    QBrush backgroundBrush(Qt::white);
    QPainter painter(this);
    painter.fillRect(0, 0, w, h, backgroundBrush);

    if (type == SOUND){

    } else {
        // MOVIE
    }

    /*int w = width(), h = height();
    QBrush markerBrush(Qt::red);
    QPen pen(markerBrush, 2);
    QPainter painter(viewport());
    painter.setPen(pen);
    for (int64_t mark : msMarker){
        painter.drawLine(mark, 0, mark, h);
    }*/
}
