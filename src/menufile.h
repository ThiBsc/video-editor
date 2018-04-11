#ifndef MENUFILE_H
#define MENUFILE_H

#include <QMenu>

class MenuFile : public QMenu
{
    Q_OBJECT

public:
    MenuFile(QWidget *parent = Q_NULLPTR);
    ~MenuFile();

signals:
    void filesImported(QStringList files);
    void quit();

public slots:
    void importFiles();
    void quitClicked();

private:
    QAction *actAddRush;
    QAction *actQuit;

};

#endif // MENUFILE_H
