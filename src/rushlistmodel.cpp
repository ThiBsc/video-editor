#include "rushlistmodel.h"

#include <QUrl>
#include <QMimeData>
#include <QFileInfo>
#include <QIcon>

RushListModel::RushListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

RushListModel::~RushListModel()
{

}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#data
 */
QVariant RushListModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (index.isValid()){
        switch (role) {
        case Qt::DisplayRole:
            ret = rushItems.at(index.row()).canonicalUrl().fileName();
            break;
        case Qt::DecorationRole:
            ret = QIcon(":/icon/video.png");
            break;
        case Qt::ToolTipRole:
            ret = rushItems.at(index.row()).canonicalUrl().fileName();
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
bool RushListModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    bool ret = false;
    if (data->hasUrls()){
        QList<QUrl> urls = data->urls();
        int idx = 0;
        bool hasMovie = false;
        while (idx < urls.size() && !hasMovie){
            QString ext = QFileInfo(urls.at(idx++).path()).completeSuffix();
            hasMovie = ext.contains(QRegExp("avi|mkv|mp4"));
        }
        ret = hasMovie;
    }
    return ret;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#dropMimeData
 */
bool RushListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(column);
    bool ret = false;
    if (data->hasUrls()){
        QList<QMediaContent> movies;
        for (QUrl url : data->urls()){
            QString ext = QFileInfo(url.path()).completeSuffix();
            if (ext.contains(QRegExp("avi|mkv|mp4"))){
                movies.append(QMediaContent(url));
            }
        }
        int before = rushItems.size();
        beginInsertRows(parent, parent.row(), parent.row()+movies.size()-1);
        rushItems.append(movies);
        if (before < rushItems.size()){
            ret = true;
            emit rushAdded(movies);
        }
        endInsertRows();
    }
    return ret;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#flags
 */
Qt::ItemFlags RushListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = QAbstractListModel::flags(index);
    if (!index.isValid()){
        ret |= Qt::ItemIsDropEnabled;
    }
    return ret;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#rowCount
 */
int RushListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rushItems.size();
}
