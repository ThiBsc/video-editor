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
    vLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(vLayout);

    toolbarActions = new QToolBar("Track action", this);

    actTrash = toolbarActions->addAction(QIcon("://icon/trash.svg"), "Trash");
    actTrashBegin = toolbarActions->addAction(QIcon("://icon/trashBegin.svg"), "TrashBegin");
    actTrashEnd = toolbarActions->addAction(QIcon("://icon/trashEnd.svg"), "TrashEnd");
    actMute = toolbarActions->addAction(QIcon("://icon/volume-off.svg"), "Mute");
    actCut = toolbarActions->addAction(QIcon("://icon/cut.svg"), "Cut");
    //actMerge = toolbarActions->addAction(QIcon("://icon/merge.svg"), "Merge");
    //actEdit = toolbarActions->addAction(QIcon("://icon/edit.svg"), "Edit");
    //actLink = toolbarActions->addAction(QIcon("://icon/link.svg"), "Link");
    toolbarActions->addSeparator();
    actDefaultTrack = toolbarActions->addAction(QIcon("://icon/default_track.svg"), "Default track zoom");

    soundTrack = new Track(this);

    vLayout->addWidget(toolbarActions);
    vLayout->addWidget(soundTrack);

    connect(actDefaultTrack, SIGNAL(triggered(bool)), soundTrack, SLOT(defaultScale()));
}

TrackTool::~TrackTool()
{
    delete actTrash;
    delete actTrashBegin;
    delete actTrashEnd;
    delete actMute;
    delete actCut;
    //delete actMerge;
    //delete actEdit;
    //delete actLink;
    delete actDefaultTrack;
    delete toolbarActions;

    delete soundTrack;

    delete vLayout;
}

/**
 * @brief Track::addMarker
 * @param ms
 * Add a time marker at ms on the track
 */
void TrackTool::addMarker(qint64 ms)
{
    soundTrack->addMarker(ms);
}

QToolBar *TrackTool::getToolbar()
{
    return toolbarActions;
}

Track *TrackTool::getTrack()
{
    return soundTrack;
}

void TrackTool::setMedia(const Media &media)
{
    soundTrack->setSource(media);
}

void TrackTool::emitActionClick(QAction *button)
{
    // Récupération de la zone ou marqueur de selection
    QVector<QTime> selected;
    selected.append(soundTrack->getSelectionTime(Track::SelectionX::X1));
    selected.append(soundTrack->getSelectionTime(Track::SelectionX::X2));

    // Récupération de l'enum correspondant à l'action
    Actions::enumActions action = Actions::enumActions::NONE;
    QString text = button->text();
    if (text == "Trash") {
        action = Actions::enumActions::DELETE_ZONE;
    } else if (text == "TrashBegin") {
        action = Actions::enumActions::DELETE_BEGIN;
    } else if (text == "TrashEnd") {
        action = Actions::enumActions::DELETE_END;
    } else if (text == "Mute") {
        action = Actions::enumActions::MUT;
    } else if (text == "Cut") {
        action = Actions::enumActions::SPLIT;
    }

    if (action != Actions::enumActions::NONE) {
        emit actionClick(action,selected);
    }

}
