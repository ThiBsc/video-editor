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

private:
    TrackModel *trackModel;

};

#endif // TRACK_H
