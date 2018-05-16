#include "media.h"
#include "mediafileinfo.h"

/**
 * @brief Media::getContent
 * @return The content of the media
 */
Media::Media() {}

/**
 * @brief Media::getContent
 * @return The content of the media
 */
Media::Media(QUrl url)
{
    this->path = url.path();
    this->originalName = url.fileName();
    this->name = url.fileName();
    updateDuration();
}

/**
 * @brief Media::getContent
 * @return The content of the media
 */
Media::Media(Media const& m): QObject()
{
    path = m.getPath();
    originalName = m.getOriginalName();
    name = m.getName();
    duration = m.getDuration();
}

Media Media::operator=(Media const& m)
{
    path = m.getPath();
    originalName = m.getOriginalName();
    name = m.getName();
    duration = m.getDuration();
    actions = m.getActions();
    return *this;
}

/**
 * @brief Media::updateDuration
 * @return Calcul of the duration of the media
 */
void Media::updateDuration()
{
    MediaFileInfo *mfi = new MediaFileInfo();
    mfi->find_meta_data(path.toStdString().c_str());
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

/**
 * @brief Media::getDuration
 * @return The duration of the media
 */
QTime Media::getDuration() const
{
    return this->duration;
}

/**
 * @brief Media::getActions
 * @return All actions of the media
 */
QMap<int,QString> Media::getActions() const
{
    return this->actions;
}

void Media::addAction(QPair<int,QString> command)
{
    this->actions.insert(command.first,command.second);
}
