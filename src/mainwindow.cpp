#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videoplayer.h"
#include "playercontrol.h"
#include "tracktool.h"
#include "track.h"
#include "actions.h"
#include "settingeditor.h"

#include <QListView>
#include <QGridLayout>
#include <QMediaPlayer>
#include <QProgressBar>

QSettings* MainWindow::settings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, "ua-bbm", "video-editor");
Actions* MainWindow::actions = new Actions();

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Video editor");
    setWindowIcon(QIcon("://icon/appli-icon.svg"));
    initSettings();

    // Suppression des aciennes vidéos
    deleteAllVideos();

    pgrBar = new QProgressBar(this);
    pgrBar->setRange(0, 0);
    pgrBar->hide();
    ui->statusBar->addPermanentWidget(pgrBar);

    // Tool bar
    actSettings = ui->mainToolBar->addAction(QIcon("://icon/cog.svg"), tr("Settings"));
    ui->mainToolBar->addSeparator();
    actAddRushs = ui->mainToolBar->addAction(QIcon("://icon/file-plus.svg"), tr("Add rushs"));
    actFinalVideo = ui->mainToolBar->addAction(QIcon("://icon/file-archive.svg"), tr("Generate final video"));
    actSave = ui->mainToolBar->addAction(QIcon("://icon/save.svg"), tr("Save project"));
    ui->mainToolBar->addSeparator();
    lblPlayAll.setText(tr("Play all:"));
    ui->mainToolBar->addWidget(&lblPlayAll);
    actPlayAll = ui->mainToolBar->addAction(QIcon("://icon/toggle-off.svg"), tr("Play all"));
    actPlayAll->setCheckable(true);
    ui->mainToolBar->addSeparator();
    actRenameMedia = ui->mainToolBar->addAction(QIcon("://icon/edit.svg"), tr("Rename media"));
    actRemoveMedia = ui->mainToolBar->addAction(QIcon("://icon/delete.svg"), tr("Delete media"));
    actFusionMedia = ui->mainToolBar->addAction(QIcon("://icon/merge.svg"), tr("Fusion media"));

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
    connect(rushListModel, SIGNAL(emitSelection(int,Media&)), trackTool, SLOT(setMedia(int,Media&)));
    connect(rushListModel, SIGNAL(rushAdded(Media&)), videoPlayer, SLOT(addMediaToPlaylist(Media&)));
    connect(rushListModel, SIGNAL(rushRemoved(int)), videoPlayer, SLOT(removeMediaToPlaylist(int)));
    connect(rushListModel, SIGNAL(rushMoved(int,int)), videoPlayer, SLOT(moveMediaInPlaylist(int,int)));
    connect(rushListModel, SIGNAL(rushRenamed(int,Media&)), videoPlayer, SLOT(renameMedia(int,Media&)));
    connect(rushListModel, SIGNAL(emitSelection(int,Media&)), videoPlayer, SLOT(setCurrentMedia(int,Media&)));
    connect(rushListModel, SIGNAL(selectionTypeChange(RushListModel::SelectionType)), this, SLOT(selectionActionChanged(RushListModel::SelectionType)));
    connect(trackTool, SIGNAL(actionClick(Actions::enumActions,QVector<QTime>)), rushListModel, SLOT(updateMedia(Actions::enumActions, QVector<QTime>)));
    connect(trackTool, SIGNAL(markerChanged(QSet<qint64>)), rushListModel, SLOT(changeCurrentMediaMarkers(QSet<qint64>)));
    connect(trackTool, SIGNAL(actionNoiseGlobal(QVector<QTime>)), rushListModel, SLOT(updateNoiseAllMedia(QVector<QTime>)));
    connect(trackTool->getTrack(), SIGNAL(requestTimeFromPlayerAfterError()), videoPlayer, SLOT(timeAsked()));
    connect(videoPlayer, SIGNAL(sendDuration(qint64)), trackTool->getTrack(), SLOT(setTimeAfterDecoderError(qint64)));
    connect(videoPlayer->getMediaPlayer(), SIGNAL(positionChanged(qint64)), trackTool->getTrack(), SLOT(updateCursorVideo(qint64)));
    connect(videoPlayer, SIGNAL(currentMediaChanged(int)), this, SLOT(changeRushListSelection(int)));
    connect(actSettings, SIGNAL(triggered(bool)), this, SLOT(openSettings()));
    connect(actAddRushs, SIGNAL(triggered(bool)), this, SLOT(importFiles()));
    connect(actSave, SIGNAL(triggered(bool)), rushListModel, SLOT(saveProject()));
    connect(actFinalVideo, SIGNAL(triggered(bool)), rushListModel, SLOT(getFinalVideo()));
    connect(actRemoveMedia, SIGNAL(triggered(bool)), rushListModel, SLOT(removeSelectedMedia()));
    connect(actRenameMedia, SIGNAL(triggered(bool)), rushListModel, SLOT(renameSelectedMedia()));
    connect(actFusionMedia, SIGNAL(triggered(bool)), rushListModel, SLOT(fusionSelectedMedia()));
    connect(actPlayAll, SIGNAL(triggered(bool)), this, SLOT(changePlayMode()));
    // Connect pour la gestion de l'attente
    connect(actions, SIGNAL(workInProgress(bool)), pgrBar, SLOT(show()));
    connect(actions, SIGNAL(workInProgress(bool)), ui->centralWidget, SLOT(setDisabled(bool)));
    connect(actions, SIGNAL(workInProgress(bool)), ui->mainToolBar, SLOT(setDisabled(bool)));
    connect(actions, SIGNAL(workFinished(bool)), pgrBar, SLOT(hide()));
    connect(actions, SIGNAL(workFinished(bool)), ui->centralWidget, SLOT(setEnabled(bool)));
    connect(actions, SIGNAL(workFinished(bool)), ui->mainToolBar, SLOT(setEnabled(bool)));
    connect(actions, SIGNAL(fusionSuccess(QString)), rushListModel, SLOT(finishFusion(QString)));
    connect(actions, SIGNAL(canApplyNoiseProfile()), rushListModel, SLOT(applyNoiseProfile()));
    connect(actions, SIGNAL(canUpdateCurrentMedia()), rushListModel, SLOT(finishUpdateMedia()));
    connect(actions, SIGNAL(canUpdateSplitMedia(QString)), rushListModel, SLOT(finishUpdateSplitMedia(QString)));
    selectionActionChanged(RushListModel::NOTHING);
    resize(600, 500);
}

MainWindow::~MainWindow()
{
    delete ui;

    delete pgrBar;

    delete actSettings;
    delete actAddRushs;
    delete actSave;
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
    deleteAllVideos();

    settings->sync();
    delete settings;
    delete actions;
}

void MainWindow::deleteAllVideos()
{
    // Suppression des fichiers vidéos des dossiers de gestion (original, preview)
    QString nameDir = MainWindow::settings->value("General/dir_original").toString();
    Actions::removeAllFileDir(nameDir);
    nameDir = MainWindow::settings->value("General/dir_preview").toString();
    Actions::removeAllFileDir(nameDir);
}

void MainWindow::importFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files"), QDir::home().canonicalPath(), "Movies (*.mkv *.avi *.mp4)");
    if (!files.isEmpty()) {
        rushListModel->addRushs(files);
    }
}

void MainWindow::changePlayMode()
{
    if (actPlayAll->isChecked()){
        actPlayAll->setIcon(QIcon("://icon/toggle-on.svg"));
        videoPlayer->setPlayMode(QMediaPlaylist::PlaybackMode::Sequential);
    } else {
        actPlayAll->setIcon(QIcon("://icon/toggle-off.svg"));
        videoPlayer->setPlayMode(QMediaPlaylist::PlaybackMode::CurrentItemOnce);
    }
}

void MainWindow::changeRushListSelection(int i)
{
    listRush->setCurrentIndex(rushListModel->index(i));
}

void MainWindow::openSettings()
{
    SettingEditor *setting_editor = new SettingEditor(this);
    if (setting_editor->exec() == QMessageBox::Accepted){
        setting_editor->saveSettings();
    }
    delete setting_editor;
}

void MainWindow::selectionActionChanged(RushListModel::SelectionType type)
{
    // actAddRushs always enable
    switch (type) {
        case RushListModel::SINGLE:
            trackTool->setEnabled(true);
            trackTool->getTrack()->graph()->setVisible(true);
            videoPlayer->getPlayerControl()->enablePlayerControl(true);
            actRenameMedia->setEnabled(true);
            actRemoveMedia->setEnabled(true);
            actFusionMedia->setEnabled(false);
            break;
        case RushListModel::MULTI:
            trackTool->setEnabled(false);
            trackTool->getTrack()->graph()->setVisible(false);
            trackTool->getTrack()->replot();
            videoPlayer->getPlayerControl()->enablePlayerControl(false);
            actRenameMedia->setEnabled(false);
            actFusionMedia->setEnabled(true);
            actRemoveMedia->setEnabled(true);
            break;
        case RushListModel::NOTHING:
            trackTool->setEnabled(false);
            trackTool->getTrack()->graph()->setVisible(false);
            trackTool->getTrack()->replot();
            videoPlayer->getPlayerControl()->enablePlayerControl(false);
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
        settings->setValue("sensibility", "0.21");
#ifdef Q_OS_WIN
        settings->setValue("prog_ffmpeg", "");
        settings->setValue("prog_sox", "");
#else
        settings->setValue("prog_ffmpeg", "ffmpeg");
        settings->setValue("prog_sox", "");
#endif
        settings->endGroup();
        settings->sync();
    }
    QDir dir;
    if (!QDir(settings->value("General/dir_preview", preview_path).toString()).exists()) {
        dir.mkpath(settings->value("General/dir_preview", preview_path).toString());
    }
    if (!QDir(settings->value("General/dir_original", original_path).toString()).exists()) {
        dir.mkpath(settings->value("General/dir_original", original_path).toString());
    }
    Actions::ffmpeg = settings->value("General/prog_ffmpeg", "ffmpeg").toString();
    Actions::sox = settings->value("General/prog_sox", "").toString();
}
