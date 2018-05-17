#ifndef TRACKTOOL_H
#define TRACKTOOL_H

#include <QWidget>
#include "media.h"
#include "actions.h"
#include "qcustomplot.h"

class QToolBar;
class QAction;
class QVBoxLayout;
class QSlider;
class Track;

/**
 * @brief The Track class
 * A widget to manage video operation
 * http://doc.qt.io/qt-5/qlistview.html
 */
class TrackTool : public QWidget
{
    Q_OBJECT

public:
    TrackTool(QWidget *parent = Q_NULLPTR);
    ~TrackTool();
    void addMarker(int64_t ms);
    QToolBar *getToolbar();
    Track *getTrack();

public slots:
    void setMedia(const QString file, qint64 duration);
    void emitActionClick(QAction *button);

signals:
    void actionClick(Actions::enumActions action,QVector<QTime> selected);

private:
    QVBoxLayout *vLayout;

    QToolBar *toolbarActions;
    QAction *actTrash;
    QAction *actTrashBegin;
    QAction *actTrashEnd;
    QAction *actMute;
    QAction *actCut;
    QAction *actMerge;
    QAction *actEdit;
    QAction *actLink;
    QAction *actDefaultTrack;

    Track *soundTrack;

};

#endif // TRACKTOOL_H
