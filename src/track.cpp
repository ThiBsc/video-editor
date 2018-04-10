#include "track.h"
#include "trackmodel.h"

Track::Track(QWidget *parent)
    : QListView(parent)
{
    setFlow(QListView::LeftToRight);
    setMaximumHeight(50);

    trackModel = new TrackModel(this);
    setModel(trackModel);
}

Track::~Track()
{

}
