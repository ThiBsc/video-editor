#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menufile.h"
#include "rushlistmodel.h"
#include "videoplayer.h"
#include "playercontrol.h"
#include "tracktool.h"
#include "actions.h"

#include <QListView>
#include <QGridLayout>

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
    connect(rushListModel, SIGNAL(emitSelection(QString,qint64)), trackTool, SLOT(setMedia(QString,qint64)));
    connect(rushListModel, SIGNAL(emitSelection(QString,qint64)), videoPlayer, SLOT(setCurrentMedia(QString,qint64)));
    connect(rushListModel, SIGNAL(disableTrackTool(bool)), trackTool, SLOT(setDisabled(bool)));
    connect(trackTool, SIGNAL(actionClick(Actions::enumActions,QVector<QTime>)), rushListModel, SLOT(updateMedia(Actions::enumActions, QVector<QTime>)));
    connect(trackTool->getToolbar(), SIGNAL(actionTriggered(QAction *)), trackTool, SLOT(emitActionClick(QAction *)));
    connect(mnuFile, SIGNAL(filesImported(QStringList)), rushListModel, SLOT(addRushs(QStringList)));
    connect(mnuFile, SIGNAL(quit()), this, SLOT(close()));
    resize(600, 500);
}

MainWindow::~MainWindow()
{
    delete ui;

    delete mnuFile;
    delete rushListModel;
    delete listRush;
    delete videoPlayer;
    delete trackTool;
    delete gLayout;

    settings->sync();
    delete settings;
}

void MainWindow::initSettings()
{
    // Si première utilisation
    if (settings->allKeys().isEmpty()){
        settings->beginGroup("general");
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
