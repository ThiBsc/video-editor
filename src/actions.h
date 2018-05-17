#ifndef ACTIONS_H
#define ACTIONS_H

#include <QObject>
#include <QTime>
#include <QString>
#include <QVector>

#include "media.h"

class Actions : public QObject
{
public:
    enum enumActions {NONE, DELETE_ZONE, DELETE_BEGIN, DELETE_END, MUT, SPLIT, FUSION};

    Actions();
    static QString getCommandOnVideo(Actions::enumActions action, QString nameVideo, QTime start, QTime end=QTime());
    static QString fusionVideos(QString finalName, QStringList nameOfVideos);
    static bool removeFile(QStringList nameOfVideos);
    static bool copyFile(QString src, QString dest);
    bool executeCommand(QString command);
    ~Actions();
};

#endif // ACTIONS_H
