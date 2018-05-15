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
    double getMicrosecFromX(int x);

public slots:
    void setBuffer();
    void plot();
    void defaultScale();
    void onXRangeChange(const QCPRange& range);
    void onReplotIsFinished();

protected:
    bool restoreSelectionCoordToPX();
    void saveSelectionCoordToRestore();
    void mouseMoveEvent(QMouseEvent *evt) override;
    void mousePressEvent(QMouseEvent *evt) override;
    void paintEvent(QPaintEvent *evt) override;
    void keyPressEvent(QKeyEvent *evt) override;
    void keyReleaseEvent(QKeyEvent *evt) override;
    void resizeEvent(QResizeEvent *evt) override;

private:
    struct Selection
    {
        double x1, x2;
    } curSelection;

    bool ctrlPressed;
    bool updateSelection;
    double timePerBytes;

    qreal getPeakValue(const QAudioFormat& format);
    TrackType type;

    QSet<int64_t> markers;

    QAudioDecoder *decoder;
    QAudioBuffer buffer;
    QVector<double> samples;
    QCPGraph *wavePlot;

};

#endif // TRACK_H
