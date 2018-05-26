#ifndef MEDIA_H
#define MEDIA_H

#include <QTime>
#include <QObject>
#include <QMap>
#include <QSet>
#include <QUrl>

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
        bool operator==(Media const& media);

        QString getPath() const;
        QString currentPath() const;
        QString getPreviewPath() const;
        QString getOriginalName() const;
        QString getName() const;
        void setName(const QString& newname);
        QTime getDuration() const;

        void findMarkers();
        void updateDuration();
        void addAction(QPair<int,QString> command);
        QMap<int,QString> getActions() const;
        QSet<qint64> getMarkers() const;
        void setMarkers(QSet<qint64> markers);
        int getNbMarkers() const;

    private:
        QString path;
        QString previewPath;
        QString originalName;
        QString name;
        QTime duration;
        QMap<int,QString> actions;
        QSet<qint64> markers;
        int nMarkers;

};

#endif // MEDIA_H
