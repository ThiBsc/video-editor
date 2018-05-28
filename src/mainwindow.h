#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QLabel>
#include "rushlistmodel.h"

namespace Ui {
    class MainWindow;
}

class QListView;
class QGridLayout;
class MenuFile;
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

public slots:
    void selectionActionChanged(RushListModel::SelectionType type);
    void importFiles();
    void changePlayMode();
    void changeRushListSelection(int i);
    void openSettings();
    void deleteAllVideos();

private:
    void initSettings();
    Ui::MainWindow *ui;

    QGridLayout *gLayout;
    TrackTool *trackTool;
    QListView *listRush;
    RushListModel *rushListModel;

    VideoPlayer *videoPlayer;

    QAction *actSettings;
    QAction *actAddRushs;
    QAction *actSave;
    QLabel lblPlayAll;
    QAction *actPlayAll;
    QAction *actRemoveMedia;
    QAction *actRenameMedia;
    QAction *actFusionMedia;
    QAction *actFinalVideo;

};

#endif // MAINWINDOW_H
