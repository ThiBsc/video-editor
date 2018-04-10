#include "trackmodel.h"

TrackModel::TrackModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

TrackModel::~TrackModel()
{

}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#data
 */
QVariant TrackModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (index.isValid()){
        switch (role) {
        case Qt::DisplayRole:
            ret = videos.at(index.row()).canonicalUrl().fileName();
            break;
        default:
            break;
        }
    }
    return ret;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#canDropMimeData
 */
bool TrackModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    return QAbstractListModel::canDropMimeData(data, action, row, column, parent);
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#dropMimeData
 */
bool TrackModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    return QAbstractListModel::dropMimeData(data, action, row, column, parent);
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#flags
 */
Qt::ItemFlags TrackModel::flags(const QModelIndex &index) const
{
    return QAbstractListModel::flags(index);
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#rowCount
 */
int TrackModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return videos.size();
}
