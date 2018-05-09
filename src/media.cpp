#include "media.h"

/**
 * @brief Media::getContent
 * @return The content of the media
 */
Media::Media(QMediaContent content)
{
    this->content= content;
}

QMediaContent Media::getContent()
{
    return this->content;
}

/**
 * @brief Media::getDuration
 * @return The duration of the media
 */
QTime Media::getDuration()
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
