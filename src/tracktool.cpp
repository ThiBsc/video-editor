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

    toolBtnTrash = new QToolButton(this);
    toolBtnTrash->setText(tr("Remove"));
    toolBtnTrash->setIcon(QIcon("://icon/trash.svg"));
    toolBtnTrash->setPopupMode(QToolButton::InstantPopup);
    actTrashBegin = new QAction(tr("Remove before"), toolBtnTrash);
    actTrashArea = new QAction(tr("Remove area"), toolBtnTrash);
    actTrashEnd = new QAction(tr("Remove after"), toolBtnTrash);

    toolBtnTrash->addAction(actTrashBegin);
    toolBtnTrash->addAction(actTrashArea);
    toolBtnTrash->addAction(actTrashEnd);
    toolbarActions->addWidget(toolBtnTrash);

    actMute = toolbarActions->addAction(QIcon("://icon/volume-off.svg"), tr("Mute"));
    actCut = toolbarActions->addAction(QIcon("://icon/cut.svg"), tr("Split"));
    actTrim = toolbarActions->addAction(QIcon("://icon/expand.svg"), tr("Trim"));
    toolbarActions->addSeparator();
    actDefaultTrack = toolbarActions->addAction(QIcon("://icon/default_track.svg"), tr("Default track zoom"));

    soundTrack = new Track(this);

    vLayout->addWidget(toolbarActions);
    vLayout->addWidget(soundTrack);

    connect(actDefaultTrack, SIGNAL(triggered(bool)), soundTrack, SLOT(defaultScale()));
    connect(toolBtnTrash, SIGNAL(triggered(QAction*)), this, SLOT(emitActionClick(QAction*)));
    connect(toolbarActions, SIGNAL(actionTriggered(QAction*)), this, SLOT(emitActionClick(QAction*)));
}

TrackTool::~TrackTool()
{
    delete actTrashBegin;
    delete actTrashArea;
    delete actTrashEnd;
    delete toolBtnTrash;
    delete actMute;
    delete actCut;
    delete actTrim;
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

void TrackTool::setMedia(int i, Media &media)
{
    Q_UNUSED(i);
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
    if (button == actTrashArea) {
        action = Actions::enumActions::DELETE_ZONE;
    } else if (button == actTrashBegin) {
        action = Actions::enumActions::DELETE_BEGIN;
    } else if (button == actTrashEnd) {
        action = Actions::enumActions::DELETE_END;
    } else if (button == actMute) {
        action = Actions::enumActions::MUT;
    } else if (button == actCut) {
        action = Actions::enumActions::SPLIT;
    } else if (button == actTrim) {
        action = Actions::enumActions::TRIM;
    }

    if (action != Actions::enumActions::NONE) {
        emit actionClick(action,selected);
    }

}

