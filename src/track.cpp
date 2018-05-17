#include "track.h"

#include <QAudioDecoder>
#include <numeric>

Track::Track(QWidget *parent)
    : QCustomPlot(parent)
    , decoder(new QAudioDecoder(this))
{
    curSelection.x1 = curSelection.x2 = -1;
    ctrlPressed = false;
    updateSelection = false;
    timePerBytes = 0.0;
    bytesPerFrame = channelCount = 0;
    cursorVideo = 0;
    setMouseTracking(ctrlPressed);

    wavePlot = addGraph();
    yAxis->setRange(QCPRange(-1, 1));

    setMinimumWidth(500);
    setMinimumHeight(100);
    setInteraction(QCP::iRangeDrag, true);
    setInteraction(QCP::iRangeZoom, true);
    axisRect()->setRangeDragAxes(xAxis, yAxis);
    axisRect()->setRangeZoomAxes(xAxis, yAxis);
    axisRect()->setRangeDrag(Qt::Horizontal);
    axisRect()->setRangeZoom(Qt::Horizontal);

    connect(decoder, SIGNAL(bufferReady()), this, SLOT(setBuffer()));
    connect(decoder, SIGNAL(finished()), this, SLOT(plot()));
    connect(xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onXRangeChange(QCPRange)));
    connect(this, SIGNAL(afterReplot()), this, SLOT(onReplotIsFinished()));
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
    markers.clear();
    samples.clear();
    decoder->setSourceFilename(fileName);
    decoder->start();
}

QTime Track::getSelectionTime(SelectionX x)
{
    QTime ret;
    int64_t micros_x = -1;
    if (x == X1){
        micros_x = curSelection.x1 < curSelection.x2 ? curSelection.x1 : curSelection.x2;
    } else {
        micros_x = curSelection.x1 < curSelection.x2 ? curSelection.x2 : curSelection.x1;
    }
    micros_x = getMicrosecFromX(micros_x);
    if (micros_x != -1){
        long ms   = (long) (micros_x / 1000) % 1000;
        long sec  = (((long) (micros_x / 1000) - ms)/1000)%60 ;
        long min  = (((((long) (micros_x / 1000) - ms)/1000) - sec)/60) %60 ;
        long hour = ((((((long) (micros_x / 1000) - ms)/1000) - sec)/60) - min)/60;
        ret = QTime(hour, min, sec, ms);
    }
    return ret;
}

void Track::setBuffer()
{
    buffer = decoder->read();
    QAudioFormat format = buffer.format();
    if (format.isValid()){
        int count = buffer.sampleCount() / buffer.format().bytesPerFrame();
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
    double duration = buffer.duration();
    timePerBytes = duration / buffer.frameCount();
    bytesPerFrame = buffer.format().bytesPerFrame();
    channelCount = buffer.format().channelCount();
    QVector<double> x(samples.size());
    std::iota(x.begin(), x.end(), 0.0);
    wavePlot->setData(x, samples, true);
    xAxis->setRange(QCPRange(0, samples.size()));
    replot();
}

/**
 * @brief Track::defaultScale
 * Reset the scale to default
 */
void Track::defaultScale()
{
    xAxis->rescale();
    replot();
}

/**
 * @brief Track::onXRangeChange
 * @param range
 * To avoid drag outside the range
 */
void Track::onXRangeChange(const QCPRange &range)
{
    if (!samples.isEmpty()){
        curSelection.x1 = curSelection.x2 = -1;
        if (range.size() < samples.size()){
            bool replace = false;
            QCPRange r = range;
            if (range.lower < 0){
                r.lower = 0;
                r.upper = 0 + range.size();
                replace = true;
            } else if (range.upper > samples.size()){
                r.upper = samples.size();
                r.lower = r.upper - range.size();
                replace = true;
            }
            if (replace){
                xAxis->setRange(r);
            }
        } else {
            xAxis->setRange(0, samples.size());
        }
    }
}

void Track::onReplotIsFinished()
{
    if (updateSelection){
        updateSelection = false;
        if (restoreSelectionCoordToPX())
            update();
    }
}

void Track::updateCursorVideo(qint64 ms)
{
    cursorVideo = ms;
    update();
}

bool Track::restoreSelectionCoordToPX()
{
    bool needUpdate = false;
    if (curSelection.x1 != -1){
        curSelection.x1 = xAxis->coordToPixel(curSelection.x1);
        needUpdate = true;
    }
    if (curSelection.x2 != -1){
        curSelection.x2 = xAxis->coordToPixel(curSelection.x2);
        needUpdate = true;
    }
    return needUpdate;
}

void Track::saveSelectionCoordToRestore()
{
    if (curSelection.x1 != -1){
        curSelection.x1 = xAxis->pixelToCoord(curSelection.x1);
        updateSelection = true;
    }
    if (curSelection.x2 != -1){
        curSelection.x2 = xAxis->pixelToCoord(curSelection.x2);
        updateSelection = true;
    }
}

/**
 * @brief Track::mouseMoveEvent
 * @param evt
 * To manage drag and zone selection
 */
void Track::mouseMoveEvent(QMouseEvent *evt)
{
    if (ctrlPressed){
        QCustomPlot::mouseMoveEvent(evt);
    } else {
        int x = evt->x();
        curSelection.x2 = x;
        update();
    }
}

/**
 * @brief Track::mousePressEvent
 * @param evt
 * To manage drag and single selection
 */
void Track::mousePressEvent(QMouseEvent *evt)
{
    if (ctrlPressed){
        QCustomPlot::mousePressEvent(evt);
    } else {
        if (evt->button() == Qt::RightButton){
            curSelection.x1 = curSelection.x2 = -1;
        } else {
            int x = evt->x();
            curSelection.x1 = x;
            curSelection.x2 = -1;
        }
        update();
    }
}

/**
 * @brief Track::paintEvent
 * @param evt
 * To draw selection
 */
void Track::paintEvent(QPaintEvent *evt)
{
    QCustomPlot::paintEvent(evt);

    QPainter painter(this);
    if (curSelection.x1 != -1){
        painter.setPen(QColor(Qt::gray));
        painter.drawLine(curSelection.x1, 0, curSelection.x1, height());
        if (curSelection.x2 != -1){
            int rectWidth = curSelection.x2 - curSelection.x1;
            painter.fillRect(curSelection.x1, 0, rectWidth, height(), QColor(20, 20, 20, 50));
            painter.drawLine(curSelection.x2, 0, curSelection.x2, height());
        }
    }

    QCPAxisRect *rect = xAxis->axisRect();
    if (!samples.isEmpty()){
        painter.setPen(QColor(Qt::red));
        for (int64_t marker : markers){
            double x_value = getXFromMS(marker);
            int x_pixel = xAxis->coordToPixel(x_value);
            if (xAxis->range().contains(x_value)){
                painter.drawLine(x_pixel, rect->top(), x_pixel, rect->bottom());
            }
        }
    }

    if (cursorVideo != 0){
        painter.setPen(QColor(Qt::darkGreen));
        double x_value = getXFromMS(cursorVideo);
        int x_pixel = xAxis->coordToPixel(x_value);
        if (xAxis->range().contains(x_value)){
            painter.drawLine(x_pixel, rect->top(), x_pixel, rect->bottom());
        }
    }
}

/**
 * @brief Track::keyPressEvent
 * @param evt
 * To use both QCustomPlot and our control
 */
void Track::keyPressEvent(QKeyEvent *evt)
{
    QCustomPlot::keyPressEvent(evt);
    int key = evt->key();
    if (key == Qt::Key_Control){
        ctrlPressed = true;
        setMouseTracking(ctrlPressed);
    }
}

void Track::keyReleaseEvent(QKeyEvent *evt)
{
    QCustomPlot::keyReleaseEvent(evt);
    int key = evt->key();
    if (key == Qt::Key_Control){
        ctrlPressed = false;
        setMouseTracking(ctrlPressed);
    }
}

void Track::resizeEvent(QResizeEvent *evt)
{
    saveSelectionCoordToRestore();
    QCustomPlot::resizeEvent(evt);
}

double Track::getXFromMS(int64_t ms)
{
    return (ms*1000) / (timePerBytes*(bytesPerFrame/channelCount));
}

int64_t Track::getMicrosecFromX(int x)
{
    return x != -1 ? ((xAxis->pixelToCoord(x)*timePerBytes) * (bytesPerFrame/channelCount)) : -1.0;
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
