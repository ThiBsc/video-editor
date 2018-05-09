#ifndef TRACKTOOL_H
#define TRACKTOOL_H

#include <QWidget>

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
    enum MarkerType { BOTH=0, SOUND, MOVIE };
    TrackTool(QWidget *parent = Q_NULLPTR);
    ~TrackTool();
    void addMarker(int64_t ms, MarkerType marker_type);

private:
    QVBoxLayout *vLayout;

    QToolBar *toolbarActions;
    QAction *actTrash;
    QAction *actMute;
    QAction *actCut;
    QAction *actMerge;
    QAction *actEdit;
    QAction *actLink;
    QAction *actZoomIn;
    QSlider *sliderZoom;
    QAction *actZoomOut;

    Track *soundTrack;
    Track *movieTrack;

};

#endif // TRACKTOOL_H
