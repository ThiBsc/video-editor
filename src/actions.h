#ifndef ACTIONS_H
#define ACTIONS_H

#include <QThread>
#include <QTime>
#include <QString>
#include <QVector>

#include "media.h"

class Actions : public QThread
{
    Q_OBJECT

public:
    static QString ffmpeg;
    static QString sox;
    static qint64 count;
    enum enumActions {NONE, DELETE_ZONE, DELETE_BEGIN, DELETE_END, MUT, TRIM, SPLIT, FUSION, NOISE};
    enum AfterAction { COPY=0, REMOVE, UPDATE_SUCCESS, SPLIT_SUCCESS, FUSION_SUCCESS, CREATE_NOISE_PROFILE };

    Actions();
    static QString getCommandOnVideo(Actions::enumActions action, QString nameVideo, QTime start, QTime end=QTime());
    static QString fusionVideos(QString finalName, QStringList nameOfVideos);
    static QString createProfileNoise(QString videoName, QTime start, QTime end);
    static QString getCommandApplyFilterNoise(QString videoName);
    static bool removeAllFileDir(const QString &dirName);
    static bool removeFile(QStringList nameOfVideos);
    static bool copyFile(QString src, QString dest);
    static bool renameFile(QString src, QString newname);
    bool executeCommand(QString command);
    ~Actions();
    void start(const QString& cmd, const QString& if_error, QVector<QPair<AfterAction, QStringList> > &if_success, QVector<QPair<AfterAction, QStringList> > &after);

signals:
    void workInProgress(bool disable);
    void workFinished(bool enable);
    void fusionSuccess(QString newName);
    void canApplyNoiseProfile();
    void canUpdateCurrentMedia();
    void canUpdateSplitMedia(QString newName);

protected:
    void run() override;

private:
    QString error_msg;
    QString command;
    QVector<QPair<AfterAction, QStringList> > after_success;
    QVector<QPair<AfterAction, QStringList> > after_exec;


};

#endif // ACTIONS_H
