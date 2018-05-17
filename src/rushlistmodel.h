#ifndef RUSHLISTMODEL_H
#define RUSHLISTMODEL_H

#include <QAbstractListModel>
#include <QMediaContent>
#include <QTime>
#include "media.h"
#include "actions.h"

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
    void managePartSplit(QString url);

public slots:
    void addRushs(QStringList files);
    void currentItemChanged(QModelIndex idx);
    void updateMedia(Actions::enumActions,QVector<QTime>);
    
signals:
    void totalDurationChanged(qint64 duration);
    void rushAdded(Media rush);
    void emitSelection(const QString file, const qint64 duration);

private:
    qint64 rushsDuration;
    QModelIndex curentIndex;
    QVector<Media> rushItems;

};

#endif // RUSHLISTMODEL_H
