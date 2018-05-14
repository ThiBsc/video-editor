#include "track.h"

#include <QAudioDecoder>
#include <numeric>

Track::Track(Track::TrackType type, QWidget *parent)
    : QCustomPlot(parent)
    , decoder(new QAudioDecoder(this))
{
    this->type = type;

    curSelection.x1 = curSelection.x2 = -1;

    wavePlot = addGraph();
    yAxis->setRange(QCPRange(-1, 1));

    setMinimumHeight(100);
    setMouseTracking(false);

    connect(decoder, SIGNAL(bufferReady()), this, SLOT(setBuffer()));
    connect(decoder, SIGNAL(finished()), this, SLOT(plot()));
}

Track::~Track()
{
    delete decoder;
    // wavePlot delete auto ?
}

void Track::addMarker(int64_t ms)
{
    markers.insert(ms);
}

void Track::setSource(const QString &fileName)
{
    samples.clear();
    decoder->setSourceFilename(fileName);
    decoder->start();
}

void Track::setBuffer()
{
    buffer = decoder->read();
    QAudioFormat format = buffer.format();
    if (format.isValid()){
        int count = buffer.sampleCount() / 2;
        QAudioFormat::SampleType sampletype = format.sampleType();
        if (sampletype == QAudioFormat::Float){
            const QAudioBuffer::S32F *data = buffer.constData<QAudioBuffer::S32F>();
            for (int i=0; i<count; i++)
                samples.append(data[i].average());
        } else {
            qreal peak = getPeakValue(format);
            const qint16 *data = buffer.constData<qint16>();
            for (int i=0; i<count; i++){
                double val = data[i]/peak;
                samples.append(val);
            }
        }
    }
}

void Track::plot()
{
    QVector<double> x(samples.size());
    std::iota(x.begin(), x.end(), 0.0);
    wavePlot->setData(x, samples, true);
    xAxis->setRange(QCPRange(0, samples.size()));
    replot();
}

void Track::mouseMoveEvent(QMouseEvent *evt)
{
    int x = evt->x();
    curSelection.x2 = x;
    update();
}

void Track::mousePressEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::RightButton){
        curSelection.x1 = -1;
        curSelection.x2 = -1;
    } else {
        int x = evt->x();
        curSelection.x1 = x;
        curSelection.x2 = -1;
    }
    update();
}

void Track::paintEvent(QPaintEvent *evt)
{
    QCustomPlot::paintEvent(evt);

    if (curSelection.x1 != -1){
        QPainter painter(this);
        painter.setPen(QColor(Qt::gray));
        painter.drawLine(curSelection.x1, 0, curSelection.x1, height());
        if (curSelection.x2 != -1){
            int rectWidth = curSelection.x2 - curSelection.x1;
            painter.fillRect(curSelection.x1, 0, rectWidth, height(), QColor(20, 20, 20, 50));
            painter.drawLine(curSelection.x2, 0, curSelection.x2, height());
        }
    }
}

/**
 * https://stackoverflow.com/questions/46947668/draw-waveform-from-raw-data-using-qaudioprobe
 * @brief Track::getPeakValue
 * @param format
 * @return The peak value
 */
qreal Track::getPeakValue(const QAudioFormat &format)
{
    qreal ret(0);
    if (format.isValid()){
        switch (format.sampleType()) {
            case QAudioFormat::Unknown:
            break;
            case QAudioFormat::Float:
                if (format.sampleSize() != 32) // other sample formats are not supported
                    ret = 0;
                else
                    ret = 1.00003;
            break;
            case QAudioFormat::SignedInt:
                if (format.sampleSize() == 32)
#ifdef Q_OS_WIN
                    ret = INT_MAX;
#endif
#ifdef Q_OS_UNIX
                    ret = SHRT_MAX;
#endif
                else if (format.sampleSize() == 16)
                    ret = SHRT_MAX;
                else if (format.sampleSize() == 8)
                    ret = CHAR_MAX;
                break;
            case QAudioFormat::UnSignedInt:
                if (format.sampleSize() == 32)
                    ret = UINT_MAX;
                else if (format.sampleSize() == 16)
                    ret = USHRT_MAX;
                else if (format.sampleSize() == 8)
                    ret = UCHAR_MAX;
            break;
        default:
            break;
        }
    }
    return ret;
}
