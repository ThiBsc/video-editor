#ifndef TRACKTOOL_H
#define TRACKTOOL_H

#include <QWidget>
#include "rushlistmodel.h"
#include "media.h"
#include "actions.h"
#include "qcustomplot.h"
#include "track.h"

class QToolBar;
class QAction;
class QVBoxLayout;
class QSlider;

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
    void addMarker(qint64 ms);
    QToolBar *getToolbar();
    Track *getTrack();

public slots:
    void setMedia(int i, Media &media);
    void emitActionClick(QAction *button);
    void activePossibleAction(Track::SelectionType stype);

signals:
    void actionClick(Actions::enumActions action,QVector<QTime> selected);
    void actionNoiseGlobal(QVector<QTime> selected);

private:
    QVBoxLayout *vLayout;

    QToolBar *toolbarActions;
    QToolButton *toolBtnTrash;
    QToolButton *toolBtnNoise;
    QAction *actTrashBegin;
    QAction *actTrashArea;
    QAction *actTrashEnd;
    QAction *actNoiseLocal;
    QAction *actNoiseGlobal;
    QAction *actMute;
    QAction *actCut;
    QAction *actTrim;
    QAction *actDefaultTrack;

    Track *soundTrack;

};

#endif // TRACKTOOL_H
