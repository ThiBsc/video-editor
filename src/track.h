#ifndef TRACK_H
#define TRACK_H

#include <QWidget>
#include <QSet>

class Track : public QWidget
{
    Q_OBJECT

public:
    enum TrackType { SOUND=0, MOVIE };
    Track(TrackType type, QWidget *parent = Q_NULLPTR);
    ~Track();
    void addMarker(int64_t ms);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    TrackType type;

    QSet<int64_t> markers;

};

#endif // TRACK_H
