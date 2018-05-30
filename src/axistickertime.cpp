#include "axistickertime.h"

AxisTickerTime::AxisTickerTime()
    : QCPAxisTicker()
    , timePerBytes(0.0)
    , bytesPerFrame(0)
    , channelCount(0)
{

}

void AxisTickerTime::setTrackInfo(double tpb, int bpf, int cc)
{
    timePerBytes = tpb;
    bytesPerFrame = bpf;
    channelCount = cc;
}

QString AxisTickerTime::getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision)
{
    QString ret;
    if (timePerBytes != 0.0){
        qint64 us = ((tick*timePerBytes) * (bytesPerFrame/channelCount));
        long ms   = (long) (us / 1000) % 1000;
        long sec  = (((long) (us / 1000) - ms)/1000)%60 ;
        long min  = (((((long) (us / 1000) - ms)/1000) - sec)/60) %60 ;
        long hour = ((((((long) (us / 1000) - ms)/1000) - sec)/60) - min)/60;
        if (hour == 0){
            ret = QTime(hour, min, sec, ms).toString("mm:ss.zz");
        } else {
            ret = QTime(hour, min, sec, ms).toString("hh:mm:ss.zz");
        }
    } else {
        ret = QCPAxisTicker::getTickLabel(tick, locale, formatChar, precision);
    }
    return ret;
}

