#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menufile.h"
#include "rushlistmodel.h"
#include "videoplayer.h"
#include "playercontrol.h"
#include "tracktool.h"
#include "track.h"
#include "actions.h"

#include <QListView>
#include <QGridLayout>
#include <QMediaPlayer>

QSettings* MainWindow::settings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, "ua-bbm", "video-editor");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Video editor");
    initSettings();

    mnuFile = new MenuFile(this);
    ui->menuBar->addMenu(mnuFile);

    actRemoveMedia = ui->mainToolBar->addAction(QIcon("://icon/delete.svg"), "Delete media");

    gLayout = new QGridLayout();
    ui->centralWidget->setLayout(gLayout);

    trackTool = new TrackTool(this);

    listRush = new QListView(this);
    rushListModel = new RushListModel(listRush);
    listRush->setModel(rushListModel);
    listRush->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listRush->setDragEnabled(true);
    listRush->setAcceptDrops(true);
    listRush->setMaximumWidth(200);

    videoPlayer = new VideoPlayer(this);

    gLayout->addWidget(listRush, 0, 0);
    gLayout->addWidget(videoPlayer, 0, 1);
    gLayout->addWidget(trackTool, 1, 0, 1, 2);

    connect(listRush->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), rushListModel, SLOT(currentSelectionChanged(QItemSelection,QItemSelection)));
    connect(rushListModel, SIGNAL(emitSelection(Media&)), trackTool, SLOT(setMedia(Media&)));
    connect(rushListModel, SIGNAL(emitSelection(Media&)), videoPlayer, SLOT(setCurrentMedia(Media&)));
    connect(rushListModel, SIGNAL(disableTrackTool(bool)), trackTool, SLOT(setDisabled(bool)));
    connect(trackTool, SIGNAL(actionClick(Actions::enumActions,QVector<QTime>)), rushListModel, SLOT(updateMedia(Actions::enumActions, QVector<QTime>)));
    connect(videoPlayer->getMediaPlayer(), SIGNAL(positionChanged(qint64)), trackTool->getTrack(), SLOT(updateCursorVideo(qint64)));
    connect(actRemoveMedia, SIGNAL(triggered(bool)), rushListModel, SLOT(removeSelectedMedia()));
    connect(mnuFile, SIGNAL(filesImported(QStringList)), rushListModel, SLOT(addRushs(QStringList)));
    connect(mnuFile, SIGNAL(quit()), this, SLOT(close()));
    resize(600, 500);
}

MainWindow::~MainWindow()
{
    delete ui;

    delete actRemoveMedia;
    delete mnuFile;
    delete rushListModel;
    delete listRush;
    delete videoPlayer;
    delete trackTool;
    delete gLayout;

    // Suppression des fichiers vidéos des dossiers de gestion (originalSplit, preview)
    QString nameDir = MainWindow::settings->value("General/dir_originalsplit").toString();
    Actions::removeAllFileDir(nameDir);
    nameDir = MainWindow::settings->value("General/dir_preview").toString();
    Actions::removeAllFileDir(nameDir);

    settings->sync();
    delete settings;
}

void MainWindow::initSettings()
{
    // Si première utilisation
    if (settings->allKeys().isEmpty()){
        settings->beginGroup("General");
        QString preview_path = QString(QDir::homePath()+"/."+settings->applicationName()+"/preview");
        QDir dir;
        if (dir.mkpath(preview_path))
            settings->setValue("dir_preview", preview_path);
        QString originalsplit_path = QString(QDir::homePath()+"/."+settings->applicationName()+"/originalsplit");
        if (dir.mkpath(originalsplit_path))
            settings->setValue("dir_originalsplit", originalsplit_path);
        settings->endGroup();
        settings->sync();
    }
}
