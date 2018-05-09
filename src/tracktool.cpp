#include "tracktool.h"
#include "track.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QSlider>
#include <QAction>

TrackTool::TrackTool(QWidget *parent)
    : QWidget(parent)
{
    vLayout = new QVBoxLayout();
    setLayout(vLayout);

    toolbarActions = new QToolBar("Track action", this);
    sliderZoom = new QSlider(Qt::Horizontal, this);
    sliderZoom->setRange(0, 100);

    actTrash = toolbarActions->addAction(QIcon("://icon/trash.svg"), "Trash");
    actMute = toolbarActions->addAction(QIcon("://icon/volume-off.svg"), "Mute");
    actCut = toolbarActions->addAction(QIcon("://icon/cut.svg"), "Cut");
    actMerge = toolbarActions->addAction(QIcon("://icon/merge.svg"), "Merge");
    actEdit = toolbarActions->addAction(QIcon("://icon/edit.svg"), "Edit");
    actLink = toolbarActions->addAction(QIcon("://icon/link.svg"), "Link");
    toolbarActions->addSeparator();
    actZoomOut = toolbarActions->addAction(QIcon("://icon/zoom-out.svg"), "Zoom-Out");
    toolbarActions->addWidget(sliderZoom);
    actZoomIn = toolbarActions->addAction(QIcon("://icon/zoom-in.svg"), "Zoom-In");

    soundTrack = new Track(Track::SOUND, this);
    movieTrack = new Track(Track::MOVIE, this);

    vLayout->addWidget(toolbarActions);
    vLayout->addWidget(soundTrack);
    vLayout->addWidget(movieTrack);
}

TrackTool::~TrackTool()
{
    delete actTrash;
    delete actMute;
    delete actCut;
    delete actMerge;
    delete actEdit;
    delete actLink;
    delete actZoomIn;
    delete sliderZoom;
    delete actZoomOut;
    delete toolbarActions;

    delete soundTrack;
    delete movieTrack;

    delete vLayout;
}

/**
 * @brief Track::addMarker
 * @param ms
 * Add a time marker at ms on the track
 */
void TrackTool::addMarker(int64_t ms, MarkerType marker_type)
{
    if (marker_type == SOUND){
        soundTrack->addMarker(ms);
    } else if (marker_type == MOVIE) {
        movieTrack->addMarker(ms);
    } else {
        // BOTH
        soundTrack->addMarker(ms);
        movieTrack->addMarker(ms);
    }
}
