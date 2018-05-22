#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videoplayer.h"
#include "playercontrol.h"
#include "tracktool.h"
#include "track.h"
#include "actions.h"

#include <QListView>
#include <QGridLayout>
#include <QMediaPlayer>

QSettings* MainWindow::settings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, "ua-bbm", "video-editor");

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Video editor");
    initSettings();

    // Tool bar
    actAddRushs = ui->mainToolBar->addAction(QIcon("://icon/file-plus.svg"), "Add rushs");
    actFinalVideo = ui->mainToolBar->addAction(QIcon("://icon/file-archive.svg"), "Generate final media");
    ui->mainToolBar->addSeparator();
    actRemoveMedia = ui->mainToolBar->addAction(QIcon("://icon/delete.svg"), "Delete media");
    actRenameMedia = ui->mainToolBar->addAction(QIcon("://icon/edit.svg"), "Rename media");
    actFusionMedia = ui->mainToolBar->addAction(QIcon("://icon/merge.svg"), "Fusion media");

    // Organisation
    trackTool = new TrackTool(this);    
    listRush = new QListView(this);
    rushListModel = new RushListModel(listRush);
    listRush->setModel(rushListModel);
    listRush->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listRush->setDragEnabled(true);
    listRush->setAcceptDrops(true);
    listRush->setFixedWidth(200);
    videoPlayer = new VideoPlayer(this);

    gLayout = new QGridLayout();
    gLayout->addWidget(listRush, 0, 0);
    gLayout->addWidget(videoPlayer, 0, 1);
    gLayout->addWidget(trackTool, 1, 0, 1, 2);
    ui->centralWidget->setLayout(gLayout);

    // Connect 
    connect(listRush->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), rushListModel, SLOT(currentSelectionChanged(QItemSelection,QItemSelection)));
    connect(rushListModel, SIGNAL(emitSelection(Media&)), trackTool, SLOT(setMedia(Media&)));
    connect(rushListModel, SIGNAL(emitSelection(Media&)), videoPlayer, SLOT(setCurrentMedia(Media&)));
    connect(rushListModel, SIGNAL(selectionTypeChange(RushListModel::SelectionType)), this, SLOT(selectionActionChanged(RushListModel::SelectionType)));
    connect(trackTool, SIGNAL(actionClick(Actions::enumActions,QVector<QTime>)), rushListModel, SLOT(updateMedia(Actions::enumActions, QVector<QTime>)));
    connect(videoPlayer->getMediaPlayer(), SIGNAL(positionChanged(qint64)), trackTool->getTrack(), SLOT(updateCursorVideo(qint64)));
    connect(actAddRushs, SIGNAL(triggered(bool)), this, SLOT(importFiles()));
    connect(actFinalVideo, SIGNAL(triggered(bool)), rushListModel, SLOT(getFinalVideo()));
    connect(actRemoveMedia, SIGNAL(triggered(bool)), rushListModel, SLOT(removeSelectedMedia()));
    connect(actRenameMedia, SIGNAL(triggered(bool)), rushListModel, SLOT(renameSelectedMedia()));
    connect(actFusionMedia, SIGNAL(triggered(bool)), rushListModel, SLOT(fusionSelectedMedia()));
    selectionActionChanged(RushListModel::NOTHING);
    resize(600, 500);
}

void MainWindow::importFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Select files", QDir::home().canonicalPath(), "Movies (*.mkv *.avi *.mp4);;Audio (*.mp3 *.wav *.ogg);;All files (*.*)");
    if (!files.isEmpty()) {
        rushListModel->addRushs(files);
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    delete actAddRushs;
    delete actRemoveMedia;
    delete actFusionMedia;
    delete actRenameMedia;
    delete actFinalVideo;
    delete rushListModel;
    delete listRush;
    delete videoPlayer;
    delete trackTool;
    delete gLayout;

    // Suppression des fichiers vidéos des dossiers de gestion (original, preview)
    QString nameDir = MainWindow::settings->value("General/dir_original").toString();
    Actions::removeAllFileDir(nameDir);
    nameDir = MainWindow::settings->value("General/dir_preview").toString();
    Actions::removeAllFileDir(nameDir);

    settings->sync();
    delete settings;
}

void MainWindow::selectionActionChanged(RushListModel::SelectionType type)
{
    // actAddRushs always enable
    switch (type) {
        case RushListModel::SINGLE:
            trackTool->setEnabled(true);
            trackTool->getTrack()->graph()->setVisible(true);
            actRenameMedia->setEnabled(true);
            actRemoveMedia->setEnabled(true);
            actFusionMedia->setEnabled(false);
            break;
        case RushListModel::MULTI:
            trackTool->setEnabled(false);
            trackTool->getTrack()->graph()->setVisible(false);
            trackTool->getTrack()->replot();
            actRenameMedia->setEnabled(false);
            actFusionMedia->setEnabled(true);
            actRemoveMedia->setEnabled(true);
            break;
        case RushListModel::NOTHING:
            trackTool->setEnabled(false);
            trackTool->getTrack()->graph()->setVisible(false);
            trackTool->getTrack()->replot();
            actFusionMedia->setEnabled(false);
            actRenameMedia->setEnabled(false);
            actRemoveMedia->setEnabled(false);
            break;
        default:
            break;
    }
}

void MainWindow::initSettings()
{
    // Si première utilisation
    QString preview_path = QString(QDir::homePath()+"/."+settings->applicationName()+"/preview");
    QString original_path = QString(QDir::homePath()+"/."+settings->applicationName()+"/original");
    if (settings->allKeys().isEmpty()) {
        settings->beginGroup("General");
        settings->setValue("dir_preview", preview_path);
        settings->setValue("dir_original", original_path);
#ifdef Q_OS_WIN
        settings->setValue("prog_ffmpeg", "ffmpeg");
#else
        settings->setValue("prog_ffmpeg", "ffmpeg");
#endif
        settings->endGroup();
        settings->sync();
    }
    QDir dir;
    dir.setCurrent(settings->value("dir_preview", preview_path).toString());
    if (!dir.exists()) {
        dir.mkpath(settings->value("dir_preview", preview_path).toString());
    }
    dir.setCurrent(settings->value("dir_original", original_path).toString());
    if (!dir.exists()) {
        dir.mkpath(settings->value("dir_original", original_path).toString());
    }
    Actions::ffmpeg = settings->value("prog_ffmpeg", "ffmpeg").toString();
}
