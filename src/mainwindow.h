#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

namespace Ui {
    class MainWindow;
}

class QListView;
class QGridLayout;
class MenuFile;
class RushListModel;
class VideoPlayer;
class TrackTool;

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
    static QSettings *settings;

private:
    void initSettings();
    Ui::MainWindow *ui;

    QGridLayout *gLayout;
    TrackTool *trackTool;
    QListView *listRush;
    RushListModel *rushListModel;

    VideoPlayer *videoPlayer;

    MenuFile *mnuFile;
    QAction *actRemoveMedia;
    QAction *actRenameMedia;
    QAction *actFusionMedia;

};

#endif // MAINWINDOW_H
