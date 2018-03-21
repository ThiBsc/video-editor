#ifndef RUSHLISTMODEL_H
#define RUSHLISTMODEL_H

#include <QAbstractListModel>
#include <QMediaContent>

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
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;

signals:
    void rushAdded(QList<QMediaContent> rush);

private:
    QList<QMediaContent> rushItems;

};

#endif // RUSHLISTMODEL_H
