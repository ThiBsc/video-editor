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
        Media();    
        Media(QMediaContent content);
        Media(Media const& media);
        QMediaContent getContent() const;
        QTime getDuration() const;
        void setDuration(QTime duration);

    private:
        QMediaContent content;
        QTime duration;
};

#endif // MEDIA_H
