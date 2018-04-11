#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QListView;
class QGridLayout;
class MenuFile;
class RushListModel;
class VideoPlayer;
class Track;

/**
 * @brief The MainWindow class
 * The MainWindow UI
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QGridLayout *gLayout;
    Track *videoTrack;
    QListView *listRush;
    RushListModel *rushListModel;

    VideoPlayer *videoPlayer;

    MenuFile *mnuFile;

};

#endif // MAINWINDOW_H
