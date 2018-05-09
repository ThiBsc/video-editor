#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QAbstractListModel>
#include <QMediaContent>
#include <QTime>
#include "media.h"

/**
 * @brief The TrackModel class
 * A list model to manage the display of Track class
 * http://doc.qt.io/qt-5/qabstractlistmodel.html
 */
class TrackModel : public QAbstractListModel
{
    Q_OBJECT

public:
    TrackModel(QObject *parent = Q_NULLPTR);
    ~TrackModel();

    QVariant data(const QModelIndex &index, int role) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    qint64 getTrackDuration() const;
    qint64 calculTrackDuration();

signals:
    void totalDurationChanged(qint64 duration);
    void rushAdded(QMediaContent rush);

private:
    qint64 trackDuration;
    QVector<Media> videos;

};

#endif // TRACKMODEL_H
