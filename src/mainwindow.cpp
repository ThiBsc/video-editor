#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menufile.h"
#include "rushlistmodel.h"
#include "videoplayer.h"
#include "playercontrol.h"
#include "track.h"

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

    videoTrack = new Track(this);

    listRush = new QListView(this);
    rushListModel = new RushListModel(listRush);
    listRush->setModel(rushListModel);
    listRush->setSelectionMode(QAbstractItemView::SingleSelection);
    listRush->setDragEnabled(true);
    listRush->setAcceptDrops(true);
    listRush->setMaximumWidth(200);

    videoPlayer = new VideoPlayer(this);

    gLayout->addWidget(listRush, 0, 0, 2, 1);
    gLayout->addWidget(videoTrack, 0, 1);
    gLayout->addWidget(videoPlayer, 1, 1);

    connect(listRush->model(), SIGNAL(totalDurationChanged(qint64)), videoPlayer->getPlayerControl(), SLOT(updateMaxDuration(qint64)));
    connect(mnuFile, SIGNAL(filesImported(QStringList)), rushListModel, SLOT(addRushs(QStringList)));
    connect(mnuFile, SIGNAL(quit()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;

    delete mnuFile;
    delete rushListModel;
    delete listRush;
    delete videoPlayer;
    delete videoTrack;
    delete gLayout;
}
