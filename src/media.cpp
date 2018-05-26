#include "media.h"
#include "mediafileinfo.h"
#include "mainwindow.h"
#include <QDir>
#include <iostream>

/**
 * @brief Media::getContent
 * @return The content of the media
 */
Media::Media()
{
    this->nMarkers = 0;
}

/**
 * @brief Media::getContent
 * @return The content of the media
 */
Media::Media(QUrl url)
{
    this->path = url.path();
    this->previewPath = MainWindow::settings->value("General/dir_preview").toString()+"/";
    this->originalName = url.fileName();
    this->name = url.fileName();
    this->nMarkers = 0;
    updateDuration();
}

/**
 * @brief Media::getContent
 * @return The content of the media
 */
Media::Media(Media const& m): QObject()
{
    path = m.getPath();
    previewPath = m.getPreviewPath();
    originalName = m.getOriginalName();
    name = m.getName();
    markers = m.getMarkers();
    nMarkers = m.getNbMarkers();
    updateDuration();
}

Media Media::operator=(Media const& m)
{
    path = m.getPath();
    previewPath = m.getPreviewPath();
    originalName = m.getOriginalName();
    name = m.getName();
    updateDuration();
    actions = m.getActions();
    markers = m.getMarkers();
    nMarkers = m.getNbMarkers();
    return *this;
}

bool Media::operator==(Media const& m)
{
    return (m.getPath() == this->getPath() && m.currentPath() == this->currentPath());
}

/**
 * @brief Media::updateDuration
 * @return Calcul of the duration of the media
 */
void Media::updateDuration()
{
    MediaFileInfo *mfi = new MediaFileInfo();
    QString goodPath = this->path;
    if (QFile(currentPath()).exists()) {
        goodPath = currentPath();
    }
    mfi->find_meta_data(goodPath.toStdString().c_str());
    this->duration = QTime(mfi->getHour(), mfi->getMinute(), mfi->getSecond(), mfi->getUSecond());
    delete mfi;
}

/**
 * @brief Media::getPath
 * @return The path of the media
 */
QString Media::getPath() const
{
    return this->path;
}

/**
 * @brief Media::getPreviewPath
 * @return The preview path of the media
 */
QString Media::getPreviewPath() const
{
    return this->previewPath;
}

/**
 * @brief Media::currentPath
 * @return The preview path of the media
 */
QString Media::currentPath() const
{
    return this->previewPath+this->name;
}

/**
 * @brief Media::getOriginalName
 * @return The original name of the media
 */
QString Media::getOriginalName() const
{
    return this->originalName;
}

/**
 * @brief Media::getName
 * @return The name of the media
 */
QString Media::getName() const
{
    return this->name;
}

void Media::setName(const QString &newname)
{
    name = newname;
}

/**
 * @brief Media::getDuration
 * @return The duration of the media
 */
QTime Media::getDuration() const
{
    return this->duration;
}

void Media::findMarkers()
{
    markers.clear();
    nMarkers = 0;
    // do something to find audio markers in movie
}

/**
 * @brief Media::getActions
 * @return All actions of the media
 */
QMap<int,QString> Media::getActions() const
{
    return this->actions;
}

QSet<qint64> Media::getMarkers() const
{
    return markers;
}

void Media::setMarkers(QSet<qint64> markers)
{
    this->markers = markers;
    nMarkers = markers.size();
}

int Media::getNbMarkers() const
{
    return nMarkers;
}

void Media::addAction(QPair<int,QString> command)
{
    this->actions.insert(command.first,command.second);
}
