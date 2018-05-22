#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
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

private:
    void initSettings();
    Ui::MainWindow *ui;

    QGridLayout *gLayout;
    TrackTool *trackTool;
    QListView *listRush;
    RushListModel *rushListModel;

    VideoPlayer *videoPlayer;

    MenuFile *mnuFile;
    QAction *actAddRushs;
    QAction *actRemoveMedia;
    QAction *actRenameMedia;
    QAction *actFusionMedia;
    QAction *actFinalVideo;

};

#endif // MAINWINDOW_H
