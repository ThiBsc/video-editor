#include "src/mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString local = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("video_editor_") + local);
    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
