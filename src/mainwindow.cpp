#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rushlistmodel.h"
#include "videoplayer.h"

#include <QListView>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    gLayout = new QGridLayout();
    ui->centralWidget->setLayout(gLayout);

    listRush = new QListView(this);
    rushListModel = new RushListModel(listRush);
    listRush->setModel(rushListModel);
    listRush->setAcceptDrops(true);
    listRush->setMaximumWidth(200);

    videoPlayer = new VideoPlayer(this);

    gLayout->addWidget(videoPlayer, 0, 1);
    gLayout->addWidget(listRush, 0, 0);

    connect(rushListModel, SIGNAL(rushAdded(QList<QMediaContent>)), videoPlayer, SLOT(addRush(QList<QMediaContent>)));
}

MainWindow::~MainWindow()
{
    delete ui;

    delete rushListModel;
    delete listRush;
    delete videoPlayer;
    delete gLayout;
}
