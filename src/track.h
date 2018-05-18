#ifndef TRACK_H
#define TRACK_H

#include "qcustomplot.h"
#include <QAudioBuffer>

class QAudioDecoder;

class Track : public QCustomPlot
{
    Q_OBJECT

public:
    enum SelectionX { X1=0, X2 };
    Track(QWidget *parent = Q_NULLPTR);
    ~Track();
    void addMarker(qint64 ms);
    void setSource(const QString &fileName);
    QTime getSelectionTime(SelectionX x);

public slots:
    void setBuffer();
    void plot();
    void defaultScale();
    void onXRangeChange(const QCPRange& range);
    void onReplotIsFinished();
    void updateCursorVideo(qint64 ms);

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
    double getXFromMS(qint64 ms);
    qint64 getMicrosecFromX(int x);
    qreal getPeakValue(const QAudioFormat& format);

    struct Selection
    {
        double x1, x2;
    } curSelection;

    bool ctrlPressed;
    bool updateSelection;
    double timePerBytes;
    int bytesPerFrame;
    int channelCount;
    qint64 cursorVideo;

    QSet<qint64> markers;

    QAudioDecoder *decoder;
    QAudioBuffer buffer;
    QVector<double> samples;
    QCPGraph *wavePlot;

};

#endif // TRACK_H
