#ifndef TRACK_H
#define TRACK_H

#include "qcustomplot.h"
#include <QAudioBuffer>

class QAudioDecoder;

class Track : public QCustomPlot
{
    Q_OBJECT

public:
    enum TrackType { SOUND=0, MOVIE };
    Track(TrackType type, QWidget *parent = Q_NULLPTR);
    ~Track();
    void addMarker(int64_t ms);
    void setSource(const QString &fileName);

public slots:
    void setBuffer();
    void plot();

protected:
    void mouseMoveEvent(QMouseEvent *evt) override;
    void mousePressEvent(QMouseEvent *evt) override;
    void paintEvent(QPaintEvent *evt) override;

private:
    struct Selection
    {
        int x1, x2;
    } curSelection;

    qreal getPeakValue(const QAudioFormat& format);
    TrackType type;

    QSet<int64_t> markers;

    QAudioDecoder *decoder;
    QAudioBuffer buffer;
    QVector<double> samples;
    QCPGraph *wavePlot;

};

#endif // TRACK_H
