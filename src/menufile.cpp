#include "menufile.h"

#include <QFileDialog>

MenuFile::MenuFile(QWidget *parent)
    : QMenu("File", parent)
{
    actAddRush = new QAction("Add rushs", this);
    actQuit = new QAction("Quit", this);

    addAction(actAddRush);
    addSeparator();
    addAction(actQuit);

    connect(actAddRush, SIGNAL(triggered(bool)), this, SLOT(importFiles()));
    connect(actQuit, SIGNAL(triggered(bool)), this, SLOT(quitClicked()));
}

MenuFile::~MenuFile()
{
    delete actAddRush;
    delete actQuit;
}

/**
 * @brief MenuFile::importFiles
 * Open a file dialog to select movie files and emit signal
 */
void MenuFile::importFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Select files", QDir::home().canonicalPath(), "Movies (*.mkv *.avi *.mp4);;All files (*.*)");
    emit filesImported(files);
}

/**
 * @brief MenuFile::quitClicked
 * emit signal quit action
 */
void MenuFile::quitClicked()
{
    emit quit();
}
