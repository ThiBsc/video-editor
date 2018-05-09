#include "media.h"

/**
 * @brief Media::getContent
 * @return The content of the media
 */
Media::Media()
{
}

/**
 * @brief Media::getContent
 * @return The content of the media
 */
Media::Media(QMediaContent content)
{
    this->content= content;
}

/**
 * @brief Media::getContent
 * @return The content of the media
 */
Media::Media(Media const& media)
{
    this->content= media.getContent();
    this->duration= media.getDuration();
}

QMediaContent Media::getContent() const
{
    return this->content;
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
 * @brief Media::getDuration
 * Set the duration of the media
 */
void Media::setDuration(QTime duration)
{
    this->duration = duration;
}
