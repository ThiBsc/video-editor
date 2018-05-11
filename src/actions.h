#ifndef ACTIONS_H
#define ACTIONS_H

#include <QProcess>
#include <QTime>
#include <QString>
#include <QVector>
#include "media.h"

class Actions : public QProcess
{
public:
    enum enumActions {DELETE_ZONE, DELETE_BEGIN, DELETE_END, MUT, SPLIT, FUSION};

    Actions();
    QString getCommandOnVideo(Actions::enumActions action, QString nameVideo, QTime start, QTime end=QTime());
    QString fusionVideos(QString finalName, QStringList nameOfVideos);
    bool removeFile(QStringList nameOfVideos);
    void executeCommand(QString command);
    ~Actions();
};

#endif // ACTIONS_H
