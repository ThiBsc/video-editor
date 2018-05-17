#ifndef RUSHLISTMODEL_H
#define RUSHLISTMODEL_H

#include <QAbstractListModel>
#include <QMediaContent>
#include <QTime>
#include <QItemSelection>
#include "media.h"
#include "actions.h"

class QAbstractItemView;

/**
 * @brief The RushListModel class
 * A list model to manage a list of "rush" movie
 * http://doc.qt.io/qt-5/qabstractlistmodel.html
 */
class RushListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    RushListModel(QAbstractItemView *parent);
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
    void managePartSplit(QString url);

public slots:
    void addRushs(QStringList files);
    void removeRush(int i);
    void currentSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void updateMedia(Actions::enumActions,QVector<QTime>);
    
signals:
    void totalDurationChanged(qint64 duration);
    void rushAdded(Media rush);
    void emitSelection(const QString file, const qint64 duration);
    void disableTrackTool(bool disable);

private:
    QAbstractItemView *parentView;
    qint64 rushsDuration;
    QModelIndex curentIndex;
    QVector<Media> rushItems;

};

#endif // RUSHLISTMODEL_H
