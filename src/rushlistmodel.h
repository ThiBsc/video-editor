#ifndef RUSHLISTMODEL_H
#define RUSHLISTMODEL_H

#include <QAbstractListModel>
#include <QMediaContent>
#include <QTime>
#include "media.h"

/**
 * @brief The RushListModel class
 * A list model to manage a list of "rush" movie
 * http://doc.qt.io/qt-5/qabstractlistmodel.html
 */
class RushListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    RushListModel(QObject *parent = Q_NULLPTR);
    ~RushListModel();

    QVariant data(const QModelIndex &index, int role) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    qint64 getTrackDuration() const;
    qint64 calculAllDuration();

public slots:
    void addRushs(QStringList files);
    
signals:
    void totalDurationChanged(qint64 duration);
    void rushAdded(Media rush);

private:
    qint64 rushsDuration;
    QVector<Media> rushItems;

};

#endif // RUSHLISTMODEL_H
