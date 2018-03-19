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

QVariant RushListModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (index.isValid()){
        switch (role) {
        case Qt::DisplayRole:
            ret = rushItems.at(index.row()).fileName();
            break;
        case Qt::DecorationRole:
            ret = QIcon(":/icon/video.png");
            break;
        case Qt::ToolTipRole:
            ret = rushItems.at(index.row()).fileName();
            break;
        default:
            break;
        }
    }
    return ret;
}

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

bool RushListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(column);
    bool ret = false;
    if (data->hasUrls()){
        QVector<QUrl> movies = QVector<QUrl>::fromList(data->urls());
        for (int i=0; i<movies.size(); i++){
            QString ext = QFileInfo(movies.at(i).path()).completeSuffix();
            if (!ext.contains(QRegExp("avi|mkv|mp4"))){
                movies.remove(i--);
            }
        }
        int before = rushItems.size();
        beginInsertRows(parent, parent.row(), parent.row()+movies.size()-1);
        rushItems.append(movies);
        if (before < rushItems.size()){
            ret = true;
        }
        endInsertRows();
    }
    return ret;
}

Qt::ItemFlags RushListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = QAbstractListModel::flags(index);
    if (!index.isValid()){
        ret |= Qt::ItemIsDropEnabled;
    }
    return ret;
}

int RushListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rushItems.size();
}
