#ifndef TRACK_H
#define TRACK_H

#include <QListView>

class TrackModel;

/**
 * @brief The Track class
 * A list view to display video track
 * http://doc.qt.io/qt-5/qlistview.html
 */
class Track : public QListView
{
    Q_OBJECT

public:
    Track(QWidget *parent = Q_NULLPTR);
    ~Track();
    void addMarker(int64_t ms);
    TrackModel *getModel();

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    TrackModel *trackModel;
    QList<int64_t> msMarker;

};

#endif // TRACK_H
