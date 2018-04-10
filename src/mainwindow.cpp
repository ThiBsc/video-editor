#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rushlistmodel.h"
#include "videoplayer.h"
#include "track.h"

#include <QListView>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Video editor");

    gLayout = new QGridLayout();
    ui->centralWidget->setLayout(gLayout);

    videoTrack = new Track(this);

    listRush = new QListView(this);
    rushListModel = new RushListModel(listRush);
    listRush->setModel(rushListModel);
    listRush->setAcceptDrops(true);
    listRush->setMaximumWidth(200);

    videoPlayer = new VideoPlayer(this);

    gLayout->addWidget(listRush, 0, 0, 2, 1);
    gLayout->addWidget(videoTrack, 0, 1);
    gLayout->addWidget(videoPlayer, 1, 1);

    connect(rushListModel, SIGNAL(rushAdded(QList<QMediaContent>)), videoPlayer, SLOT(addRush(QList<QMediaContent>)));
}

MainWindow::~MainWindow()
{
    delete ui;

    delete rushListModel;
    delete listRush;
    delete videoPlayer;
    delete videoTrack;
    delete gLayout;
}
