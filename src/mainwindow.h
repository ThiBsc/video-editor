#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QListView;
class QGridLayout;
class RushListModel;
class VideoPlayer;

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
    QListView *listRush;
    RushListModel *rushListModel;

    VideoPlayer *videoPlayer;

};

#endif // MAINWINDOW_H
