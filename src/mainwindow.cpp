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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Video editor");

    mnuFile = new MenuFile(this);
    ui->menuBar->addMenu(mnuFile);

    gLayout = new QGridLayout();
    ui->centralWidget->setLayout(gLayout);

    trackTool = new TrackTool(this);

    listRush = new QListView(this);
    rushListModel = new RushListModel(listRush);
    listRush->setModel(rushListModel);
    listRush->setSelectionMode(QAbstractItemView::SingleSelection);
    listRush->setDragEnabled(true);
    listRush->setAcceptDrops(true);
    listRush->setMaximumWidth(200);

    videoPlayer = new VideoPlayer(this);

    gLayout->addWidget(listRush, 0, 0);
    gLayout->addWidget(videoPlayer, 0, 1);
    gLayout->addWidget(trackTool, 1, 0, 1, 2);

    connect(listRush, SIGNAL(clicked(QModelIndex)), rushListModel, SLOT(currentItemChanged(QModelIndex)));
    connect(rushListModel, SIGNAL(emitSelection(QString)), trackTool, SLOT(setMedia(QString)));
    connect(rushListModel, SIGNAL(emitSelection(QString)), videoPlayer, SLOT(setCurrentMedia(QString)));    
    connect(trackTool, SIGNAL(actionClick(Actions::enumActions,QVector<QTime>)), rushListModel, SLOT(updateMedia(Actions::enumActions, QVector<QTime>)));
    connect(trackTool->getToolbar(), SIGNAL(actionTriggered(QAction *)), trackTool, SLOT(emitActionClick(QAction *)));
    connect(rushListModel, SIGNAL(totalDurationChanged(qint64)), videoPlayer->getPlayerControl(), SLOT(updateMaxDuration(qint64)));
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
}
