#ifndef MEDIA_H
#define MEDIA_H

#include <QMediaContent>
#include <QTime>

/**
 * @brief The Media class
 */
class Media
{
    public:
        Media(QMediaContent content);
        QMediaContent getContent();
        QTime getDuration();
        void setDuration(QTime duration);

    private:
        QMediaContent content;
        QTime duration;
};

#endif // MEDIA_H
