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
    enum SelectionType { NOTHING=0, SINGLE, MULTI };
    RushListModel(QAbstractItemView *parent);
    ~RushListModel();

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    QStringList mimeTypes() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    qint64 getTrackDuration() const;
    qint64 calculAllDuration();
    void manageNewVideo(QString url);
    void removeMedia(Media const m);

public slots:
    void addRushs(QStringList files);
    void removeRush(int i);
    void removeSelectedMedia();
    void fusionSelectedMedia();
    void canFinishFusion(QString newName);
    void renameSelectedMedia();
    void saveProject();
    void getFinalVideo();
    void currentSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void changeCurrentMediaMarkers(QSet<qint64> markers);
    void updateMedia(Actions::enumActions action, QVector<QTime> selected);
    void canUpdateMedia();
    void canUpdateSplitMedia(QString newName);
    void updateNoiseAllMedia(QVector<QTime> selected);
    void applyNoiseProfile();
    
signals:
    void totalDurationChanged(qint64 duration);
    void rushAdded(Media& rush);
    void rushRemoved(int i);
    void rushMoved(int a, int b);
    void emitSelection(int i, Media& media);
    void selectionTypeChange(RushListModel::SelectionType type);

private:
    QAbstractItemView *parentView;
    qint64 rushsDuration;
    QModelIndex curentIndex;
    QVector<Media> rushItems;

};

#endif // RUSHLISTMODEL_H
