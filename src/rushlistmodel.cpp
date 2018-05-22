#include "rushlistmodel.h"
#include "mediafileinfo.h"
#include "mainwindow.h"

#include <QUrl>
#include <QMimeData>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QIcon>
#include <QDataStream>
#include <QFileDialog>
#include <QSize>
#include <QMap>
#include <iostream>
#include <QAbstractItemView>

RushListModel::RushListModel(QAbstractItemView *parent)
    : QAbstractListModel(parent)
{
    parentView = parent;
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
        const Media media = rushItems.at(index.row());
        switch (role) {
            case Qt::DisplayRole:
                ret = QString("%1%2").arg(media.getNbMarkers() > 0 ? "("+QString::number(media.getNbMarkers())+") " : "", media.getName());
                break;
            case Qt::DecorationRole:
                ret = QIcon(":/icon/video.png");
                break;
            case Qt::ToolTipRole:
                ret = media.getName();
                break;
            case Qt::EditRole:
                ret = data(index, Qt::DisplayRole);
                break;
            default:
                break;
        }
    }
    return ret;
}

bool RushListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = false;
    if (index.isValid() && role == Qt::EditRole){
        Media media = rushItems.at(index.row());
        if (Actions::renameFile(media.currentPath(), media.getPreviewPath()+value.toString())){
            ret = true;
            media.setName(value.toString());
            rushItems.replace(index.row(), media);
            emit dataChanged(index, index);
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
    Q_UNUSED(column);
    bool ret = false;
    if (data->hasFormat("video-editor/rushUrl")){
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
        QByteArray encoded = data->data("video-editor/rushUrl");
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
    Qt::ItemFlags ret = QAbstractListModel::flags(index) | Qt::ItemIsEditable;
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
        bool copy = Actions::copyFile(m.getPath(), MainWindow::settings->value("General/dir_preview").toString());
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

void RushListModel::removeRush(int i)
{
    beginRemoveRows(QModelIndex(), i, i);
    Media media = rushItems.takeAt(i);
    Actions::removeFile({media.currentPath()});
    endRemoveRows();
}

void RushListModel::removeMedia(Media const m)
{
    int i = rushItems.indexOf(m);
    if (i != -1) {
        removeRush(i);
    } else {
        // Gestion erreur
        std::cout << "Erreur suppression, index inconnu" << std::endl;
    }
}

void RushListModel::removeSelectedMedia()
{
    QModelIndexList indexes = parentView->selectionModel()->selectedIndexes();
    QVector<Media> allRemoveMedia;
    for (QModelIndex index : indexes) {
        if (index.isValid()) {
            allRemoveMedia.append(rushItems.at(index.row()));
        }
    }
    for (auto media : allRemoveMedia) {
        removeMedia(media);
    }
}

void RushListModel::getFinalVideo()
{
    QString finaleName = QFileDialog::getSaveFileName(parentView, tr("Save File"), "untitled.mkv");
    if (!finaleName.isEmpty()) {
        QStringList nameOfVideos;
        for (Media rush : rushItems) {
            nameOfVideos.append(rush.currentPath());
        }
        QString tmpName = MainWindow::settings->value("General/dir_preview").toString()+"/finalVideo.mkv";
        QString command = Actions::fusionVideos(tmpName, nameOfVideos);

        Actions myAction;
        myAction.executeCommand(command);
        Actions::copyFile(tmpName, finaleName);
        QFile::remove(tmpName);
    }
}

void RushListModel::fusionSelectedMedia()
{
    // Récupération des medias valides sélectionnés
    QModelIndexList indexes = parentView->selectionModel()->selectedIndexes();
    QStringList listeVideoSelected;
    QString firstVideoName;
    for (QModelIndex index : indexes) {
        if (index.isValid()) {
            listeVideoSelected.append(rushItems.at(index.row()).currentPath());
            if (firstVideoName.isNull()) {
                firstVideoName = rushItems.at(index.row()).getName();
            }
        }
    }
    // Gestion de la fusion 
    QString newName = MainWindow::settings->value("General/dir_preview").toString()+"/fusion_"+firstVideoName;
    QString command = Actions::fusionVideos(newName, listeVideoSelected);
    Actions myAction;
    bool cmdSuccess = myAction.executeCommand(command);
    if (cmdSuccess) {
        manageNewVideo(newName);
        removeSelectedMedia();
    } else {
        // Gestion des erreurs
    }
}

void RushListModel::renameSelectedMedia()
{
    QModelIndex index = parentView->currentIndex();
    if (index.isValid())
        parentView->edit(index);
}

/**
 * @brief RushListModel::updateMedia
 * Update the preview media with action of user
 */
void RushListModel::updateMedia(Actions::enumActions action, QVector<QTime> selected)
{
    if (curentIndex.isValid() && !selected.value(0).isNull()) {
        // Récupération du média courant
        Media m = rushItems.at(curentIndex.row());
        // Création de la commande
        QString command = Actions::getCommandOnVideo(action, m.getName(), selected.value(0), selected.value(1));
        bool cmdSuccess = false;
        if (command.size() != 0) {
            // Ajout de la commande
            QPair<int,QString> actionCommand(static_cast<int>(action),command);
            m.addAction(actionCommand);
            // Exécution de l'action
            Actions myAction;
            cmdSuccess = myAction.executeCommand(command);
            m.updateDuration();
            emit emitSelection(m);
        }
        // Gestion des erreurs et cas particuliers
        if (!cmdSuccess) {
            std::cout << "Erreur dans les commandes" << std::endl;
            // emit actionError();
        } else if (action == Actions::enumActions::SPLIT) {
            QString path = MainWindow::settings->value("General/dir_preview").toString()+"/";
            manageNewVideo(path+"part_"+m.getName());
        }
    }    
}

void RushListModel::manageNewVideo(QString url)
{
    QUrl origin(url);
    // Déplace le fichier dans un dossier
    bool copy = Actions::copyFile(origin.path(), MainWindow::settings->value("General/dir_original").toString());
    if (!copy) {
        // Gestion erreur

    }
    // Suppression du fichier de preview
    Actions::removeFile({origin.path()});
    // Ajout aux rushs le nouveau fichier
    QFileInfo info(MainWindow::settings->value("General/dir_original").toString()+"/"+origin.fileName());
    QStringList newFile(info.absoluteFilePath());
    addRushs(newFile);
}

/**
 * @brief RushListModel::currentSelectionChanged
 * @param selected
 * @param deselected
 */
void RushListModel::currentSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
    curentIndex = QModelIndex();
    if (parentView->selectionModel()->selectedRows().size() > 1){
        // Selection multiple
        emit selectionTypeChange(MULTI);
    } else if (parentView->selectionModel()->selectedRows().size() == 1){
        // Selection simple
        QModelIndex idx = parentView->selectionModel()->selectedIndexes().first();
        curentIndex = idx;
        Media cur = rushItems.at(idx.row());
        emit emitSelection(cur);
        emit selectionTypeChange(SINGLE);
    } else {
        // Aucune selection
        emit selectionTypeChange(NOTHING);
    }
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
