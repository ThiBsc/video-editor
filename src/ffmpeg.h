#ifndef FFMPEG_H
#define FFMPEG_H
#include <QProcess>
#include <QTime>
#include <QVector>
#include "media.h"

class Ffmpeg : public QProcess
{
public:
    enum Actions {CUT};

    Ffmpeg();
    QString getCommandOnVideo(Ffmpeg::Actions action, QString nameVideo, QTime start, QTime duration);
    void executeCommand(QString command);
    void fusionVideos(QVector<Media> videos);
    ~Ffmpeg();
};

#endif // FFMPEG_H
