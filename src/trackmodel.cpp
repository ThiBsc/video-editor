#include "trackmodel.h"
#include "mediafileinfo.h"

#include <QMimeData>
#include <QDataStream>
#include <QSize>
#include <iostream>

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
    qint64 taille;
    qint64 prorata;
    qint64 listWidth = 750;
    if (index.isValid()){
        switch (role) {
            case Qt::DisplayRole:
                ret = videos.at(index.row()).content.canonicalUrl().fileName();
                break;
            case Qt::SizeHintRole:
                prorata =  (QTime(0, 0, 0).msecsTo(videos.at(index.row()).duration)*(quint64)100)/getTrackDuration();
                taille = (prorata*listWidth)/100;
                ret = QSize(taille,40);
                break;
            default:
                break;
        } 
    }
    return ret;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#canDropMimeData
 * Allow drop if it come from RushList or in internal
 */
bool TrackModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    bool ret = false;
    if (data->hasFormat("video-editor/rushUrl")){
        ret = true;
    } else if (!data->hasUrls() && data->hasFormat("application/x-qabstractitemmodeldatalist")){
        if (row != -1 || parent.isValid()){
            ret = true;
        }
    }
    return ret;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#dropMimeData
 */
bool TrackModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    bool ret = false;
    if (data->hasFormat("video-editor/rushUrl")){
        const QString url = data->text();
        Media m;
        MediaFileInfo *mfi = new MediaFileInfo();
        mfi->find_meta_data(url.toStdString().c_str());
        m.content = QMediaContent(QUrl(url));
        m.duration = QTime(mfi->getHour(), mfi->getMinute(), mfi->getSecond(), mfi->getUSecond());
        delete mfi;
        beginInsertRows(parent, videos.size(), videos.size());
        videos.append(m);
        endInsertRows();
        ret = true;
        calculTrackDuration();
        emit totalDurationChanged(trackDuration);
        emit rushAdded(m.content);
    } else if (!data->hasUrls() && data->hasFormat("application/x-qabstractitemmodeldatalist")){
        QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        int row_src;
        stream >> row_src;
        QModelIndex src = index(row_src);
        QModelIndex dest = index(row == -1 ? parent.row() : row);
        if (src != dest){
            beginMoveRows(src, src.row(), src.row(), dest, dest.row());
            Media m = videos.takeAt(src.row());
            if (row < videos.size())
                videos.insert(dest.row(), m);
            else
                videos.append(m);
            endMoveRows();
            ret = true;
        }
    }
    return ret;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#flags
 */
Qt::ItemFlags TrackModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = QAbstractListModel::flags(index);
    if (!index.isValid()){
        ret |= Qt::ItemIsDropEnabled;
    } else {
        ret |= Qt::ItemIsDragEnabled;
    }
    return ret;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#rowCount
 */
int TrackModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return videos.size();
}

/**
 * @brief TrackModel::getTrackDuration
 * @return The total duration of the track in ms
 */
qint64 TrackModel::getTrackDuration() const
{
    return trackDuration;
}

/**
 * @brief TrackModel::calculTrackDuration
 * @return The total duration of the track in ms
 */
qint64 TrackModel::calculTrackDuration()
{   
    std::vector<Media> track = videos.toStdVector();
    qint64 sum = std::accumulate(track.begin(), track.end(), 0, [](qint64 s, const Media &a) {
        return s + QTime(0, 0, 0).msecsTo(a.duration);
    });
    trackDuration = sum;
    return trackDuration;
}
