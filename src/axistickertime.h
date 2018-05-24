#ifndef AXISTICKERTIME_H
#define AXISTICKERTIME_H

#include "qcustomplot.h"

class AxisTickerTime : public QCPAxisTicker
{
public:
    AxisTickerTime();
    void setTrackInfo(double tpb, int bpf, int cc);

protected:
    QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) override;
    double timePerBytes;
    int bytesPerFrame;
    int channelCount;

};

#endif // AXISTICKERTIME_H
