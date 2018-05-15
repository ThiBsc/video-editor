#include "rushlistmodel.h"
#include "mediafileinfo.h"

#include <QUrl>
#include <QMimeData>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QIcon>
#include <QDataStream>
#include <QSize>
#include <QMap>
#include <iostream>
#include <QWidget>

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
    if (index.isValid()) {
        switch (role) {
            case Qt::DisplayRole:
                ret = rushItems.at(index.row()).getName();
                break;
            case Qt::DecorationRole:
                ret = QIcon(":/icon/video.png");
                break;
            case Qt::ToolTipRole:
                ret = rushItems.at(index.row()).getName();
                break;
            default:
                break;
        }
    }
    return ret;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#canDropMimeData
 * Allow drop when it's an internal move
 */
bool RushListModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    bool ret = false;
    if (data->hasFormat("video-editor/rushUrl")){
        if (row != -1 || parent.isValid()){
            ret = true;
        }
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
bool RushListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    bool ret = false;
    if (data->hasFormat("video-editor/rushUrl")) {
        QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encoded, QIODevice::ReadOnly);
        int row_src;
        stream >> row_src;
        QModelIndex src = index(row_src);
        QModelIndex dest = index(row == -1 ? parent.row() : row);
        if (src != dest) {
            beginMoveRows(src, src.row(), src.row(), dest, dest.row());
            Media m(rushItems.takeAt(src.row()));
            if (row < rushItems.size()) {
                rushItems.insert(dest.row(), m);
            } else {
                rushItems.append(m);
            }
            endMoveRows();
            ret = true;
        }
    }
    return ret;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#mimeData
 * @brief RushListModel::mimeData
 * @param indexes
 * @return mimeData
 */
QMimeData *RushListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = QAbstractItemModel::mimeData(indexes);
    QModelIndex index = indexes.first();
    if (index.isValid()){
        mimeData->setText(rushItems.at(index.row()).getPath());
    }
    return mimeData;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#mimeTypes
 * @brief RushListModel::mimeTypes
 * @return mime_types
 */
QStringList RushListModel::mimeTypes() const
{
    QStringList mime_types = QAbstractItemModel::mimeTypes();
    mime_types.prepend("video-editor/rushUrl");
    return mime_types;
}

/**
 * http://doc.qt.io/qt-5/qabstractitemmodel.html#flags
 */
Qt::ItemFlags RushListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = QAbstractListModel::flags(index);
    if (index.isValid()){
        ret |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
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

/**
 * @brief RushListModel::addRushs
 * @param files to add in the list
 */
void RushListModel::addRushs(QStringList files)
{
    QModelIndex index;
    for (const QString file : files){
        QUrl monurl(file);
        Media m(monurl);
        bool copy = RushListModel::copyFile(m);
        if (copy) {
            beginInsertRows(index, rushItems.size(), rushItems.size()+files.size());        
            rushItems.append(m);
            endInsertRows();
            emit rushAdded(m);
        }        
    }
    qint64 duration = calculAllDuration();
    emit totalDurationChanged(duration); 
}

/**
 * @brief RushListModel::copyFile
 * Copy the media in preview directory 
 */
bool RushListModel::copyFile(Media m)
{
    QString src = "../preview";
    QDir dir(src);
    if(!dir.exists()){
        return false;
    }
    QFile::copy(m.getPath(), src+"/"+m.getName());
    return true;
}

/**
 * @brief RushListModel::updateMedia
 * Update the preview media with action of user
 */
void RushListModel::updateMedia(Actions::enumActions action, QVector<QTime> selected)
{
    // Récupération du média courant
    Media m = rushItems.at(curentIndex.row());    
    // Création de la commande
    QString command = Actions::getCommandOnVideo(action, m.getName(), selected.value(0), selected.value(1));
    // Ajout de la commande
    QPair<int,QString> actionCommand(static_cast<int>(action),command);
    m.addAction(actionCommand);
    // Exécution de l'action
    Actions myAction;
    bool cmdSuccess = myAction.executeCommand(command);
    if (cmdSuccess) {

    }
}

/**
 * @brief RushListModel::currentItemChanged
 */
void RushListModel::currentItemChanged(QModelIndex idx)
{
    curentIndex = idx;
    QString path = QDir::currentPath()+"/../preview/";
    emit emitSelection(path+rushItems.at(idx.row()).getName());
}

/**
 * @brief RushListModel::getTrackDuration
 * @return The total duration of the track in ms
 */
qint64 RushListModel::getTrackDuration() const
{
    return rushsDuration;
}

/**
 * @brief RushListModel::calculAllDuration
 * @return The total duration of the track in ms
 */
qint64 RushListModel::calculAllDuration()
{   
    std::vector<Media> track = rushItems.toStdVector();
    qint64 sum = std::accumulate(track.begin(), track.end(), 0, [](qint64 s, const Media &a) {
        return s + QTime(0, 0, 0).msecsTo(a.getDuration());
    });
    rushsDuration = sum;
    return rushsDuration;
}
