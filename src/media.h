#ifndef MEDIA_H
#define MEDIA_H

#include <QTime>
#include <QObject>
#include <QMap>
#include <QUrl>
#include "actions.h"

/**
 * @brief The Media class
 */
class Media : public QObject
{
    Q_OBJECT

    public:
        Media();    
        Media(QUrl url);
        Media(Media const& media);
        Media operator=(Media const& media);

        QString getPath() const;
        QString getOriginalName() const;
        QString getName() const;
        QTime getDuration() const;

        void updateDuration();
        QMap<int,QString> getActions() const;

    private:
        QString path;
        QString originalName;
        QString name;
        QTime duration;
        QMap<int,QString> actions;
};

#endif // MEDIA_H